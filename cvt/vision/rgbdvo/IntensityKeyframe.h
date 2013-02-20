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
    class IntensityKeyframe : public RGBDKeyframe<WarpFunc> {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            typedef RGBDKeyframe<WarpFunc>              Base;
            typedef float                               T;
            typedef typename Base::Result               Result;
            typedef typename Base::JacobianType         JacobianType;
            typedef typename Base::AlignDataType        AlignDataType;
            typedef typename AlignDataType::ScreenJacobianType    ScreenJacobianType;


            IntensityKeyframe( const Matrix3f &K, size_t octaves, float scale );
            ~IntensityKeyframe();

            void updateOfflineDataForScale( AlignDataType& data,
                                            const Image& gray,
                                            const Image& depth,
                                            float scale );

            void sparseOfflineDataForScale( AlignDataType& data,
                                            ScaleFeatures& features,
                                            const Image& gray,
                                            const Image& depth,
                                            float scale );

            void addPointsOnScale( AlignDataType& data,
                                   const std::vector<Vector3f>& pts,
                                   const Matrix4f& referenceToWorld );

            void sparseOfflineData( std::vector<ScaleFeatures>& featuresForScale,
                                    const Matrix4<T>& pose,
                                    const ImagePyramid& pyramid,
                                    const Image& depth );


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
    inline void IntensityKeyframe<WarpFunc>::updateOfflineDataForScale( typename Base::AlignDataType& data,
                                                                        const Image& gray,
                                                                        const Image& depth,
                                                                        float scale )
    {
        IMapScoped<const float> depthMap( depth );
        const float* d = depthMap.ptr();
        size_t depthStride = depthMap.stride() / sizeof( float );

        Vector2f currP;
        Vector3<T> p3d;
        Eigen::Matrix<T, 2, 1> g;
        JacobianType j;
        ScreenJacobianType sj;

        // compute the image gradients
        this->computeImageGradients( data.gradX, data.gradY, gray );
        data.gray = gray;

        data.clear();
        size_t pixelsOnOctave = ( gray.width() - 1 ) * ( gray.height() - 1 );
        data.reserve( 0.4f * pixelsOnOctave );

        // TODO: replace this by a simd function!
        // temp vals
        std::vector<float> tmpx( gray.width() );
        std::vector<float> tmpy( gray.height() );
        Base::initializePointLookUps( &tmpx[ 0 ], tmpx.size(), data.intrinsics[ 0 ][ 0 ], data.intrinsics[ 0 ][ 2 ] );
        Base::initializePointLookUps( &tmpy[ 0 ], tmpy.size(), data.intrinsics[ 1 ][ 1 ], data.intrinsics[ 1 ][ 2 ] );

        IMapScoped<const float> gxMap( data.gradX );
        IMapScoped<const float> gyMap( data.gradY );
        IMapScoped<const float> grayMap( data.gray );

        WarpFunc warp;
        for( size_t y = 0; y < gray.height() - 1; y++ ){
            const float* gx = gxMap.ptr();
            const float* gy = gyMap.ptr();
            const float* value = grayMap.ptr();

            // scale the point
            currP.y = scale * y;

            for( size_t x = 0; x < gray.width() - 1; x++ ){
                currP.x = scale * x;
                float z = Base::interpolateDepth( currP, d, depthStride );
                if( z > this->_minDepth && z < 10.0f ){
                    g[ 0 ] = gx[ x ];
                    g[ 1 ] = gy[ x ];

                    float salience = Math::abs( g[ 0 ] ) + Math::abs( g[ 1 ] );
                    if( salience < Base::_gradientThreshold )
                        continue;

                    p3d[ 0 ] = tmpx[ x ] * z;
                    p3d[ 1 ] = tmpy[ y ] * z;
                    p3d[ 2 ] = z;

                    warp.screenJacobian( sj, p3d, data.intrinsics );
                    //WarpFunc::computeJacobian( j, p3d, data.intrinsics, g, value[ x ] );
                    j = g.transpose() * sj;

                    data.hessian.noalias() += j.transpose() * j;

                    data.jacobians.push_back( j );
                    data.pixelValues.push_back( value[ x ] );
                    data.points3d.push_back( p3d );
                    data.gradients.push_back( Vector2f( gx[ x ], gy[ x ] ) );
                    data.screenJacobians.push_back( sj );
                }
            }
            gxMap++;
            gyMap++;
            grayMap++;
        }

        // precompute the inverse hessian
        data.inverseHessian = data.hessian.inverse();

        std::cout << "# data points" << data.gradients.size() << std::endl;
        std::cout << "Hessian: " << data.hessian << ", inverse: " << data.inverseHessian << std::endl;

    }

    template <class WarpFunc>
    inline void IntensityKeyframe<WarpFunc>::addPointsOnScale( AlignDataType& data,
                                                               const std::vector<Vector3f>& pts,
                                                               const Matrix4f& referenceToWorld )
    {
        // reproject the points into this reference frame: we need x, y, z & u, v
        SIMD* simd = SIMD::instance();

        std::vector<Vector3f> transformed;
        transformed.resize( pts.size() );
        simd->transformPoints( &transformed[ 0 ], referenceToWorld, &pts[ 0 ], pts.size() );

        std::vector<Vector3f>::const_iterator pit;
        const std::vector<Vector3f>::const_iterator pitEnd;

        const Matrix3f& K = data.intrinsics;

        IMapScoped<const float> gxMap( data.gradX );
        IMapScoped<const float> gyMap( data.gradY );
        IMapScoped<const float> grayMap( data.gray );

        const float* gVals = grayMap.ptr();
        const float* gxVals = gxMap.ptr();
        const float* gyVals = gyMap.ptr();

        JacobianType j;
        Eigen::Matrix<T, 2, 1> g;

        WarpFunc warp;
        while( pit != pitEnd ){
            // project to screen
            Vector2f w( K * *pit );

            // we need: pixel value and gradient at that position (nearest neighbour for testing)
            int x = ( int )( w.x + 0.5f );
            int y = ( int )( w.y + 0.5f );

            // access the values: intensity, gx, gy
            size_t offset = y * grayMap.stride() + x;

            // evaluate the jacobian
            g[ 0 ] = gxVals[ offset ];
            g[ 1 ] = gyVals[ offset ];
            warp.computeJacobian( j, *pit, data.intrinsics, g, gVals[ offset ] );

            // add it to the data
            data.jacobians.push_back( j );
            data.pixelValues.push_back( gVals[ offset ] );
            data.points3d.push_back( *pit );
            data.hessian.noalias() += j.transpose() * j;

            ++pit;
        }

        data.inverseHessian = data.hessian.inverse();
    }


    template <class WarpFunc>
    inline void IntensityKeyframe<WarpFunc>::sparseOfflineDataForScale( AlignDataType& data,
                                                                        ScaleFeatures& features,
                                                                        const Image& gray,
                                                                        const Image& depth,
                                                                        float scale )
    {
        IMapScoped<const float> depthMap( depth );
        const float* d = depthMap.ptr();
        size_t depthStride = depthMap.stride() / sizeof( float );

        Vector2f currP;
        Vector3<T> p3d;
        Eigen::Matrix<T, 2, 1> g;
        JacobianType j;

        // compute the image gradients
        Image gxI, gyI;
        this->computeImageGradients( gxI, gyI, gray );

        data.clear();
        size_t pixelsOnOctave = ( gray.width() - 1 ) * ( gray.height() - 1 );
        data.reserve( 0.4f * pixelsOnOctave );

        // temp vals
        std::vector<float> tmpx( gray.width() );
        std::vector<float> tmpy( gray.height() );
        Base::initializePointLookUps( &tmpx[ 0 ], tmpx.size(), data.intrinsics[ 0 ][ 0 ], data.intrinsics[ 0 ][ 2 ] );
        Base::initializePointLookUps( &tmpy[ 0 ], tmpy.size(), data.intrinsics[ 1 ][ 1 ], data.intrinsics[ 1 ][ 2 ] );

        IMapScoped<const float> gxMap( gxI );
        IMapScoped<const float> gyMap( gyI );
        IMapScoped<const float> grayMap( gray );

        const float* gx = gxMap.ptr();
        const float* gy = gyMap.ptr();
        const float* value = grayMap.ptr();

        size_t gStride = gxMap.stride() / sizeof( float );
        size_t grayStride = grayMap.stride() / sizeof( float );

        for( size_t i = 0; i < features.positions.size(); i++ ){
            const Vector2f& pos = features.positions[ i ];

            // scale the point to highest level
            currP = scale * pos;
            float z = Base::interpolateDepth( currP, d, depthStride );
            if( z > this->_minDepth ){
                g[ 0 ] = gx[ gStride * ( int )pos.y + ( int )pos.x ];
                g[ 1 ] = gy[ gStride * ( int )pos.y + ( int )pos.x ];

                p3d[ 0 ] = tmpx[ (int)pos.x ] * z;
                p3d[ 1 ] = tmpy[ (int)pos.y ] * z;
                p3d[ 2 ] = z;

                float v = value[ grayStride * ( int )pos.y + ( int )pos.x ];

                WarpFunc::computeJacobian( j, p3d, data.intrinsics, g, v );

                data.jacobians.push_back( j );
                data.pixelValues.push_back( v );
                data.points3d.push_back( p3d );
                data.hessian.noalias() += j.transpose() * j;
            } else {
                // remember the free features
                features.nonDepthFeatures.push_back( i );
            }
        }
        data.inverseHessian = data.hessian.inverse();
    }

    template <class WarpFunc>
    inline void IntensityKeyframe<WarpFunc>::sparseOfflineData( std::vector<ScaleFeatures>& features,
                                                                const Matrix4<T>& pose,
                                                                const ImagePyramid& pyramid,
                                                                const Image& depth )
    {
        this->_pose = pose;

        float scale = 1.0f;
        for( size_t i = 0; i < pyramid.octaves(); i++ ){
            this->sparseOfflineDataForScale( Base::_dataForScale[ i ], features[ i ], pyramid[ i ], depth, scale );
            scale /= pyramid.scaleFactor();
        }
    }
}


#endif // INTENSITYKEYFRAME_H
