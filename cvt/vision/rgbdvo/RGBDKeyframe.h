/*
            CVT - Computer Vision Tools Library
            
     Copyright (c) 2012, Philipp Heise, Sebastian Klose
     
    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef RGBDKEYFRAME_H
#define RGBDKEYFRAME_H

#include <cvt/math/Matrix.h>
#include <cvt/math/SE3.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/EigenBridge.h>

namespace cvt
{
    template <class T>
    class RGBDKeyframe {
        public:
            enum PointStorage {
                STORE_RELATIVE,
                STORE_ABSOLUTE
            };

            typedef Vector3<T> PointType;
            typedef Matrix3<T> Mat3Type;

            // TODO:
            // these typedef have to go into a AlignmentCostFunc
            // e.g. atm we use std. SSD
            // for Affine Lighting, we need 2 more
            // for MI the evaluation is different
            typedef Eigen::Matrix<T, 1, 6>  JacobianType;
            typedef Eigen::Matrix<T, 6, 6>  HessianType;

            struct AlignmentData {
                std::vector<PointType>      points3d;
                std::vector<float>          pixelValues;
                std::vector<JacobianType>   jacobians;
                HessianType                 inverseHessian;
                Mat3Type                    intrinsics;

                void reserve( size_t size )
                {
                    points3d.reserve( size );
                    pixelValues.reserve( size );
                    jacobians.reserve( size );
                }

                void clear()
                {
                    points3d.clear();
                    pixelValues.clear();
                    jacobians.clear();
                }
            };

            RGBDKeyframe( const Mat3Type &K, size_t octaves, T scale, PointStorage storage = STORE_RELATIVE );

            ~RGBDKeyframe();

            void updateOfflineData( const Matrix4<T>& pose, const ImagePyramid& pyramid, const Image& depth );

            const AlignmentData& dataForScale( size_t o ) const { return _dataForScale[ o ]; }
            const Matrix4<T>&    pose()                   const { return _pose; }

            void setDepthMapScaleFactor( float scaleFactor );
            void setMinimumDepth( T depthTresh );
            void setGradientThreshold( float thresh );

            PointStorage    pointStorageType() const { return _storageType; }

        private:
            const PointStorage          _storageType;
            Matrix4<T>                  _pose;
            std::vector<AlignmentData>  _dataForScale;
            IKernel                     _kx;
            IKernel                     _ky;
            IKernel                     _gaussX;
            IKernel                     _gaussY;

            float                       _depthScaling;
            T                           _minDepth;
            float                       _gradientThreshold;

            void updateIntrinsics( const Mat3Type& K, T scale );
            void computeImageGradients( Image& gx, Image& gy, const Image& gray ) const;

    };

    template <class T>
    inline RGBDKeyframe<T>::RGBDKeyframe( const Mat3Type &K, size_t octaves, T scale, PointStorage storage ) :
        _storageType( storage ),
        _kx( IKernel::HAAR_HORIZONTAL_3 ),
        _ky( IKernel::HAAR_VERTICAL_3 ),
        _gaussX( IKernel::GAUSS_HORIZONTAL_3 ),
        _gaussY( IKernel::GAUSS_VERTICAL_3 ),
        _depthScaling( 1.0f ),
        _minDepth( 0.05 ),
        _gradientThreshold( 0.0f )
    {
        _kx.scale( -0.5 );
        _ky.scale( -0.5 );

        _dataForScale.resize( octaves );
        updateIntrinsics( K, scale );
    }

    template <class T>
    inline RGBDKeyframe<T>::~RGBDKeyframe()
    {
    }

    template <class T>
    inline void RGBDKeyframe<T>::updateIntrinsics( const Mat3Type& K, T scale )
    {
        _dataForScale[ 0 ].intrinsics = K;
        for( size_t o = 1; o < _dataForScale.size(); o++ ){
            _dataForScale[ o ].intrinsics = _dataForScale[ o - 1 ].intrinsics * scale;
            _dataForScale[ o ].intrinsics[ 2 ][ 2 ] = ( T )1.0;
        }
    }

    template <class T>
    inline void RGBDKeyframe<T>::updateOfflineData( const Matrix4<T>& poseMat,
                                                    const ImagePyramid& pyramid,
                                                    const Image& depth )
    {
        _pose = poseMat;

        Image gxI, gyI;
        float scale = 1.0f;
        Eigen::Matrix<T, 3, 1> p3d;
        Vector3<T> cvtPoint;
        Eigen::Matrix<T, 2, 1> g;
        Eigen::Matrix<T, 1, 6> j;
        typename SE3<T>::ScreenJacType J;
        HessianType H;

        IMapScoped<const float> depthMap( depth );

        for( size_t i = 0; i < pyramid.octaves(); i++ ){
            const Image& gray = pyramid[ i ];
            computeImageGradients( gxI, gyI, gray );

            AlignmentData& data = _dataForScale[ i ];
            data.clear();
            data.reserve( 0.5f * gray.width() * gray.height() );

            float invFx = 1.0f / data.intrinsics[ 0 ][ 0 ];
            float invFy = 1.0f / data.intrinsics[ 1 ][ 1 ];
            float cx    = data.intrinsics[ 0 ][ 2 ];
            float cy    = data.intrinsics[ 1 ][ 2 ];

            // temp vals
            std::vector<float> tmpx( gray.width() );
            std::vector<float> tmpy( gray.height() );

            for( size_t i = 0; i < tmpx.size(); i++ ){
                tmpx[ i ] = ( i - cx ) * invFx;
            }
            for( size_t i = 0; i < tmpy.size(); i++ ){
                tmpy[ i ] = ( i - cy ) * invFy;
            }

            IMapScoped<const float> gxMap( gxI );
            IMapScoped<const float> gyMap( gyI );
            IMapScoped<const float> grayMap( gray );

            Eigen::Matrix<T, 3, 3> K;
            EigenBridge::toEigen( K, data.intrinsics );
            SE3<T> pose;

            H.setZero();

            for( size_t y = 0; y < gray.height(); y++ ){
                const float* gx = gxMap.ptr();
                const float* gy = gyMap.ptr();
                const float* value = grayMap.ptr();

                depthMap.setLine( scale * y );
                const float* d = depthMap.ptr();
                for( size_t x = 0; x < gray.width(); x++ ){
                    float z = d[ ( size_t ) scale * x ] * _depthScaling;
                    if( z > _minDepth ){
                        g[ 0 ] = gx[ x ];
                        g[ 1 ] = gy[ x ];

                        if( g.squaredNorm() < _gradientThreshold )
                            continue;

                        cvtPoint[ 0 ] = tmpx[ x ] * z;
                        cvtPoint[ 1 ] = tmpy[ y ] * z;
                        cvtPoint[ 2 ] = z;

                        if( _storageType == STORE_ABSOLUTE ){
                            cvtPoint = _pose * cvtPoint;
                            if( cvtPoint.z < _minDepth )
                                continue;
                        }

                        EigenBridge::toEigen( p3d, cvtPoint );

                        pose.screenJacobian( J, p3d, K );

                        j = g.transpose() * J;

                        data.jacobians.push_back( j );
                        data.pixelValues.push_back( value[ x ] );                        
                        data.points3d.push_back( cvtPoint );
                        H.noalias() += j.transpose() * j;
                    }
                }
                gxMap++;
                gyMap++;
                grayMap++;
            }

            // precompute the inverse hessian
            data.inverseHessian = H.inverse();

            scale /= pyramid.scaleFactor();
        }
    }


    template <class T>
    inline void RGBDKeyframe<T>::setDepthMapScaleFactor( float scaleFactor )
    {
        _depthScaling = ( float )0xffff / scaleFactor;
    }

    template <class T>
    inline void RGBDKeyframe<T>::setMinimumDepth( T depthTresh )
    {
        _minDepth = depthTresh;
    }

    template <class T>
    inline void RGBDKeyframe<T>::setGradientThreshold( float thresh )
    {
        _gradientThreshold = Math::sqr( thresh );
    }

    template <class T>
    inline void RGBDKeyframe<T>::computeImageGradients( Image& gx, Image& gy, const Image& gray ) const
    {
        gx.reallocate( gray.width(), gray.height(), IFormat::GRAY_FLOAT );
        gy.reallocate( gray.width(), gray.height(), IFormat::GRAY_FLOAT );

        // sobel
        gray.convolve( gx, _kx, _gaussY );
        gray.convolve( gy, _gaussX, _ky );

        // normal
        //gray.convolve( gx, kx );
        //gray.convolve( gy, ky );
    }
}

#endif // RGBDKEYFRAME_H
