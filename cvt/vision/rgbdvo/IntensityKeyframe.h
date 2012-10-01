/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef INTENSITYKEYFRAME_H
#define INTENSITYKEYFRAME_H

#include <cvt/math/Matrix.h>
#include <cvt/math/SE3.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/EigenBridge.h>

#include <cvt/vision/rgbdvo/RGBDWarp.h>
#include <cvt/vision/rgbdvo/RobustWeighting.h>
#include <cvt/vision/rgbdvo/RGBDKeyframe.h>

#include<Eigen/StdVector>

namespace cvt
{
    template <class WarpFunc>
    class IntensityKeyframe : public RGBDKeyframe<WarpFunc> {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            typedef RGBDKeyframe<WarpFunc>              Base;
            typedef typename Base::T                    T;
            typedef typename Base::Result               Result;
            typedef typename Base::JacobianType         JacobianType;


            IntensityKeyframe( const Matrix3f &K, size_t octaves, float scale );
            ~IntensityKeyframe();

            void updateOfflineData( const Matrix4<T>& pose, const ImagePyramid& pyramid, const Image& depth );

        private:
            void alignSingleScaleNonRobust( Result& result, const Image& gray, const Image& depth, size_t octave );
            void alignSingleScaleRobust( Result& result, const Image& gray, const Image& depth, size_t octave );

    };

    template <class WarpFunc>
    inline IntensityKeyframe<WarpFunc>::IntensityKeyframe( const Matrix3f &K, size_t octaves, float scale ) :
        RGBDKeyframe<WarpFunc>( K, octaves, scale )
    {
    }

    template <class WarpFunc>
    inline IntensityKeyframe<WarpFunc>::~IntensityKeyframe()
    {
    }

    template <class WarpFunc>
    inline void IntensityKeyframe<WarpFunc>::updateOfflineData( const Matrix4<T>& poseMat,
                                                                          const ImagePyramid& pyramid,
                                                                          const Image& depth )
    {
        this->_pose = poseMat;

        Image gxI, gyI;
        float scale = 1.0f;

        Vector3<T> p3d;
        Eigen::Matrix<T, 2, 1> g;
        JacobianType j;

        IMapScoped<const float> depthMap( depth );
        const float* d = depthMap.ptr();
        size_t depthStride = depthMap.stride() / sizeof( float );
        Vector2f currP;

        for( size_t i = 0; i < pyramid.octaves(); i++ ){
            const Image& gray = pyramid[ i ];
            this->computeImageGradients( gxI, gyI, gray );

            typename Base::AlignDataType& data = this->_dataForScale[ i ];
            data.clear();
            size_t pixelsOnOctave = ( gray.width() - 1 ) * ( gray.height() - 1 );
            data.reserve( 0.6f * pixelsOnOctave );

            // temp vals
            std::vector<float> tmpx( gray.width() );
            std::vector<float> tmpy( gray.height() );
            Base::initializePointLookUps( &tmpx[ 0 ], tmpx.size(), data.intrinsics[ 0 ][ 0 ], data.intrinsics[ 0 ][ 2 ] );
            Base::initializePointLookUps( &tmpy[ 0 ], tmpy.size(), data.intrinsics[ 1 ][ 1 ], data.intrinsics[ 1 ][ 2 ] );

            IMapScoped<const float> gxMap( gxI );
            IMapScoped<const float> gyMap( gyI );
            IMapScoped<const float> grayMap( gray );

            data.hessian.setZero();

            for( size_t y = 0; y < gray.height() - 1; y++ ){
                const float* gx = gxMap.ptr();
                const float* gy = gyMap.ptr();
                const float* value = grayMap.ptr();

                currP.y = scale * y;

                for( size_t x = 0; x < gray.width() - 1; x++ ){
                    currP.x = scale * x;
                    float z = Base::interpolateDepth( currP, d, depthStride );
                    if( z > this->_minDepth ){
                        g[ 0 ] = gx[ x ];
                        g[ 1 ] = gy[ x ];

                        float salience = Math::abs( g[ 0 ] ) + Math::abs( g[ 1 ] );
                        if( salience < Base::_gradientThreshold )
                            continue;

                        p3d[ 0 ] = tmpx[ x ] * z;
                        p3d[ 1 ] = tmpy[ y ] * z;
                        p3d[ 2 ] = z;

                        WarpFunc::computeJacobian( j, p3d, data.intrinsics, g, value[ x ] );

                        data.jacobians.push_back( j );
                        data.pixelValues.push_back( value[ x ] );
                        data.points3d.push_back( p3d );
            //            data.hessian.noalias() += j.transpose() * j;
                    }
                }
                gxMap++;
                gyMap++;
                grayMap++;
            }


            // select best N jacobians:

            size_t numPixels = Base::_pixelPercentageToSelect * pixelsOnOctave;
            if( data.jacobians.size() <= numPixels )
                Base::updateHessian( data );
            else
                Base::selectInformation( data, numPixels );


            // precompute the inverse hessian
            data.inverseHessian = data.hessian.inverse();

            /*
            cvt::String hessString;
            float normalizer = data.jacobians.size();
            std::cout << "Octave: " << i << std::endl;
            hessString.sprintf( "Hessian: %0.2f, %0.2f, %0.2f, %0.2f, %0.2f, %0.2f",
                                data.hessian( 0, 0 )/normalizer, data.hessian( 1, 1 )/normalizer, data.hessian( 2, 2 )/normalizer,
                                data.hessian( 3, 3 )/normalizer, data.hessian( 4, 4 )/normalizer, data.hessian( 5, 5 )/normalizer );
            std::cout << hessString << std::endl;
            hessString.sprintf( "invHessian: %0.4f, %0.4f, %0.4f, %0.4f, %0.4f, %0.4f",
                                data.inverseHessian( 0, 0 ), data.inverseHessian( 1, 1 ), data.inverseHessian( 2, 2 ),
                                data.inverseHessian( 3, 3 ), data.inverseHessian( 4, 4 ), data.inverseHessian( 5, 5 ) );
            std::cout << hessString << std::endl;
            */

            scale /= pyramid.scaleFactor();
        }
    }


    template <class WarpFunc>
    inline void IntensityKeyframe<WarpFunc>::alignSingleScaleRobust( Result& result, const Image& gray, const Image&, size_t octave )
    {
        SIMD* simd = SIMD::instance();

        const typename Base::AlignDataType& kfdata = Base::_dataForScale[ octave ];

        const size_t num = kfdata.points3d.size();
        const size_t width = gray.width();
        const size_t height = gray.height();

        std::vector<Vector2<T> > warpedPts;
        warpedPts.resize( num );

        std::vector<float> interpolatedPixels;
        interpolatedPixels.resize( num );

        Matrix4f projMat;
        Matrix4<T> K4( kfdata.intrinsics );

        // sum of jacobians * delta
        typename Base::JacobianType deltaSum, jtmp;
        typename Base::HessianType  hessian;

        IMapScoped<const float> grayMap( gray );

        result.iterations = 0;
        result.numPixels = 0;
        result.pixelPercentage = 0.0f;
        while( result.iterations < Base::_maxIters ){
            // build the updated projection Matrix
            projMat = K4 * result.warp.poseMatrix();

            // project the points:
            simd->projectPoints( &warpedPts[ 0 ], projMat, &kfdata.points3d[ 0 ], num );
            simd->warpBilinear1f( &interpolatedPixels[ 0 ], &warpedPts[ 0 ].x, grayMap.ptr(), grayMap.stride(), width, height, -1.0f, num );

            deltaSum.setZero();
            hessian.setZero();

            result.pixelPercentage = 0;
            result.numPixels = 0;
            result.costs = 0.0f;
            for( size_t i = 0; i < num; i++ ){
                if( interpolatedPixels[ i ] >= 0.0f ){
                    // compute the delta
                    float delta = result.warp.computeResidual( kfdata.pixelValues[ i ], interpolatedPixels[ i ] );
                    result.costs += Math::sqr( delta );
                    result.numPixels++;

                    T weight = Base::_weighter.weight( delta );
                    jtmp = weight * kfdata.jacobians[ i ];

                    hessian.noalias() += jtmp.transpose() * kfdata.jacobians[ i ];
                    deltaSum += jtmp * delta;
                }
            }

            if( !result.numPixels ){
                // not a single pixel projected into the image
                break;
            }

            // evaluate the delta parameters
            typename WarpFunc::DeltaVectorType deltaP = -hessian.inverse() * deltaSum.transpose();
            result.warp.updateParameters( deltaP );

            result.iterations++;
            if( deltaP.norm() < Base::_minUpdate )
                break;
        }
        if( result.numPixels )
            result.pixelPercentage = ( float )result.numPixels / ( float )num;
    }

    template <class WarpFunc>
    inline void IntensityKeyframe<WarpFunc>::alignSingleScaleNonRobust( Result& result, const Image& gray, const Image&, size_t octave )
    {
        SIMD* simd = SIMD::instance();
        Matrix4f projMat;

        const typename Base::AlignDataType& kfdata = Base::_dataForScale[ octave ];
        const size_t num = kfdata.points3d.size();
        const size_t width = gray.width();
        const size_t height = gray.height();
        std::vector<Vector2<T> > warpedPts;
        warpedPts.resize( num );

        std::vector<float> interpolatedPixels;
        interpolatedPixels.resize( num );

        Matrix4<T> K4( kfdata.intrinsics );

        // sum of jacobians * delta
        typename Base::JacobianType deltaSum, jtmp;

        IMapScoped<const float> grayMap( gray );

        Matrix4f bestMat;
        bestMat = result.warp.poseMatrix();

        result.iterations = 0;
        result.numPixels = 0;
        result.pixelPercentage = 0.0f;

        while( result.iterations < Base::_maxIters ){

            // build the updated projection Matrix
            projMat = K4 * result.warp.poseMatrix();
            // project the points:
            simd->projectPoints( &warpedPts[ 0 ], projMat, &kfdata.points3d[ 0 ], num );
            simd->warpBilinear1f( &interpolatedPixels[ 0 ], &warpedPts[ 0 ].x, grayMap.ptr(), grayMap.stride(), width, height, -1.0f, num );

            deltaSum.setZero();
            result.numPixels = 0;
            result.costs = 0.0f;
            for( size_t i = 0; i < num; i++ ){
                // compute the delta
                if( interpolatedPixels[ i ] >= 0.0f ){
                    float delta = result.warp.computeResidual( kfdata.pixelValues[ i ], interpolatedPixels[ i ] );
                    result.costs += Math::sqr( delta );
                    result.numPixels++;

                    jtmp = delta * kfdata.jacobians[ i ];
                    deltaSum += jtmp;
                }
            }

            if( !result.numPixels ){
                break;
            }

            // evaluate the delta parameters
            typename WarpFunc::DeltaVectorType deltaP = -kfdata.inverseHessian * deltaSum.transpose();
            result.warp.updateParameters( deltaP );

            result.iterations++;
            if( deltaP.norm() < Base::_minUpdate )
                break;
        }

        if( result.numPixels )
            result.pixelPercentage = ( float )result.numPixels / ( float )num;
    }
}


#endif // INTENSITYKEYFRAME_H
