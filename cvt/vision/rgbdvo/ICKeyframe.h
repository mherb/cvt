/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef CVT_ICKEYFRAME_H
#define CVT_ICKEYFRAME_H

#include <cvt/math/Matrix.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/vision/rgbdvo/RGBDKeyframe.h>

#include<Eigen/StdVector>

namespace cvt
{

    static void saveGradientImages( const Image& gxI, const Image& gyI, float scale )
    {
        String file;
        file.sprintf( "gradx_%f.png", scale );
        gxI.save( file );
        file.sprintf( "grady_%f.png", scale );
        gyI.save( file );
    }

    template <class T>
    static void dumpDataInfo( const T& data, float scale )
    {
        cvt::String hessString;
        float normalizer = data.jacobians.size();
        std::cout << "Octave: " << scale << std::endl;
        hessString.sprintf( "Hessian: %0.2f, %0.2f, %0.2f, %0.2f, %0.2f, %0.2f",
                            data.hessian( 0, 0 )/normalizer, data.hessian( 1, 1 )/normalizer, data.hessian( 2, 2 )/normalizer,
                            data.hessian( 3, 3 )/normalizer, data.hessian( 4, 4 )/normalizer, data.hessian( 5, 5 )/normalizer );
        std::cout << hessString << std::endl;
        hessString.sprintf( "invHessian: %0.4f, %0.4f, %0.4f, %0.4f, %0.4f, %0.4f",
                            data.inverseHessian( 0, 0 ), data.inverseHessian( 1, 1 ), data.inverseHessian( 2, 2 ),
                            data.inverseHessian( 3, 3 ), data.inverseHessian( 4, 4 ), data.inverseHessian( 5, 5 ) );
        std::cout << hessString << std::endl;
    }

    template <class WarpFunc>
    class ICKeyframe : public RGBDKeyframe<WarpFunc> {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            typedef RGBDKeyframe<WarpFunc>              Base;
            typedef float                               T;
            typedef typename Base::JacobianType         JacobianType;
            typedef typename Base::ScreenJacobianType   ScreenJacobianType;
            typedef typename Base::JacobianVec          JacobianVec;
            typedef typename Base::AlignmentData        AlignmentData;
            typedef typename Base::GradientType         GradientType;

            ICKeyframe( const Matrix3f &K, size_t octaves, float scale );
            ~ICKeyframe();

            void recompute( std::vector<float>& residuals,
                            JacobianVec& jacobians,
                            const WarpFunc& warp,
                            const IMapScoped<const float>& gray,
                            size_t octave );
    };

    template <class WarpFunc>
    inline ICKeyframe<WarpFunc>::ICKeyframe( const Matrix3f &K, size_t octaves, float scale ) :
        RGBDKeyframe<WarpFunc>( K, octaves, scale )
    {
    }

    template <class WarpFunc>
    inline ICKeyframe<WarpFunc>::~ICKeyframe()
    {
    }

    template <class WarpFunc>
    inline void ICKeyframe<WarpFunc>::recompute( std::vector<float>& residuals,
                                                 JacobianVec& jacobians,
                                                 const WarpFunc& warp,
                                                 const IMapScoped<const float>& gray,
                                                 size_t octave )
    {
        SIMD* simd = SIMD::instance();
        const size_t width = gray.width();
        const size_t height = gray.height();
        std::vector<Vector2f> warpedPts;
        std::vector<float> interpolatedPixels;

        const AlignmentData& data = this->dataForScale( octave );
        size_t n = data.size();

        // construct the projection matrix
        Matrix4f projMat( data.intrinsics() );
        projMat *= warp.pose();

        // resize the data storage
        warpedPts.resize( n );
        interpolatedPixels.resize( n );
        residuals.resize( n );
        jacobians.resize( n );

        // project the points:
        simd->projectPoints( &warpedPts[ 0 ], projMat, &data.points()[ 0 ], n );

        // interpolate the pixel values
        simd->warpBilinear1f( &interpolatedPixels[ 0 ], &warpedPts[ 0 ].x, gray.ptr(), gray.stride(), width, height, -10.0f, n );

        // compute the residuals
        warp.computeResiduals( &residuals[ 0 ], &data.pixels()[ 0 ], &interpolatedPixels[ 0 ], n );

        // sort out bad pixels (out of image)
        const JacobianVec& refJacs = data.jacobians();
        size_t savePos = 0;
        for( size_t i = 0; i < n; ++i ){
            if( interpolatedPixels[ i ] >= 0.0f ){
                // OK
                jacobians[ savePos ] = refJacs[ i ];
                residuals[ savePos ] = residuals[ i ];
                ++savePos;
            }
        }
        residuals.erase( residuals.begin() + savePos, residuals.end() );
        jacobians.erase( jacobians.begin() + savePos, jacobians.end() );
    }
}


#endif // ICKEYFRAME_H
