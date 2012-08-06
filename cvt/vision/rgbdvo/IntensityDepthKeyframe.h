/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef INTENSITYDEPTHKEYFRAME_H
#define INTENSITYDEPTHKEYFRAME_H

#include <cvt/math/Matrix.h>
#include <cvt/math/SE3.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/EigenBridge.h>

#include <cvt/vision/rgbdvo/RGBDWarp.h>
#include <cvt/vision/rgbdvo/RobustWeighting.h>
#include <cvt/vision/rgbdvo/RGBDKeyframe.h>

#include <Eigen/StdVector>

namespace cvt
{
    template <class WarpFunc,
              class Weighter = NoWeighting<typename WarpFunc::Type> >
    class IntensityDepthKeyframe : public RGBDKeyframe<WarpFunc, Weighter> {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            typedef RGBDKeyframe<WarpFunc, Weighter>    Base;
            typedef typename Base::T                    T;
            typedef typename Base::Result               Result;
            typedef typename Base::Mat3Type             Mat3Type;

            typedef Eigen::Matrix<T, 1, 6>              DepthJacType;

            IntensityDepthKeyframe( const Mat3Type &K, size_t octaves, float scale );
            ~IntensityDepthKeyframe();

            void updateOfflineData( const Matrix4<T>& pose, const ImagePyramid& pyramid, const Image& depth );

        private:
            struct DepthData
            {
                EIGEN_MAKE_ALIGNED_OPERATOR_NEW
                std::vector<DepthJacType>   jacobians;
                typename Base::HessianType  hessian;
                T                           lambda;

                void reserve( size_t n ){ jacobians.reserve( n ); }
                void clear()            { jacobians.clear(); }
            };

            std::vector<DepthData>  _depthDataForScale;

            bool checkResult( const Result& res, const Matrix4<T> &lastPose, size_t numPixels ) const;
            void alignSingleScaleNonRobust( Result& result, const Image& gray, const Image& depth, size_t octave );
            void alignSingleScaleRobust( Result& result, const Image& gray, const Image& depth, size_t octave );

    };

    template <class WarpFunc, class Weighter>
    inline IntensityDepthKeyframe<WarpFunc, Weighter>::IntensityDepthKeyframe( const Mat3Type &K, size_t octaves, float scale ) :
        RGBDKeyframe<WarpFunc, Weighter>( K, octaves, scale )
    {
        _depthDataForScale.resize( octaves );
    }

    template <class WarpFunc, class Weighter>
    inline IntensityDepthKeyframe<WarpFunc, Weighter>::~IntensityDepthKeyframe()
    {
    }

    template <class WarpFunc, class Weighter>
    inline void IntensityDepthKeyframe<WarpFunc, Weighter>::updateOfflineData( const Matrix4<T>& poseMat,
                                                                               const ImagePyramid& pyramid,
                                                                               const Image& depth )
    {
        this->_pose = poseMat;

        Image gxI, gyI;
        float scale = 1.0f;

        Vector3<T> p3d;
        Eigen::Matrix<T, 2, 1> g;
        typename Base::JacobianType j;

        DepthJacType    dJac = DepthJacType::Zero();

        IMapScoped<const float> depthMap( depth );
        const float* dPtr = depthMap.ptr();
        size_t depthStride = depthMap.stride() / sizeof( float );
        Vector2f currP;

        for( size_t i = 0; i < pyramid.octaves(); i++ ){
            const Image& gray = pyramid[ i ];
            Base::computeImageGradients( gxI, gyI, gray );

            typename Base::AlignmentData& data = this->_dataForScale[ i ];
            DepthData & depthData = _depthDataForScale[ i ];

            data.clear();
            depthData.clear();
            data.reserve( 0.6f * gray.width() * gray.height() );
            depthData.reserve( 0.6f * gray.width() * gray.height() );

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

            data.hessian.setZero();
            data.depthHessian.setZero();
            for( size_t y = 0; y < gray.height(); y++ ){
                currP.y = scale * y;
                const float* gx = gxMap.ptr();
                const float* gy = gyMap.ptr();
                const float* value = grayMap.ptr();

                for( size_t x = 0; x < gray.width(); x++ ){
                    currP.x = scale * x;
                    float z = Base::interpolateDepth( currP, dPtr, depthStride );
                    if( z > this->_minDepth ){
                        g[ 0 ] = gx[ x ];
                        g[ 1 ] = gy[ x ];

                        // TODO: selection should be based on depth and gradient threshold
                        float salience = Math::abs( g[ 0 ] ) + Math::abs( g[ 1 ] );
                        if( salience < this->_gradientThreshold )
                            continue;

                        p3d[ 0 ] = tmpx[ x ] * z;
                        p3d[ 1 ] = tmpy[ y ] * z;
                        p3d[ 2 ] = z;

                        // the depth value jacobian:
                        // TODO: this is plain wrong?!
                        //evalDepthJacobian( dJac, p3d );

                        WarpFunc::computeJacobian( j, p3d, data.intrinsics, g, value[ x ] );

                        data.jacobians.push_back( j );
                        data.pixelValues.push_back( value[ x ] );
                        data.points3d.push_back( p3d );
                        data.hessian.noalias() += j.transpose() * j;

                        //data.depthHessian.noalias() += dJac.transpose() * dJac;
                    }
                }
                gxMap++;
                gyMap++;
                grayMap++;
            }

            // precompute the inverse hessian
            data.inverseHessian = ( data.hessian  ).inverse();

            scale /= pyramid.scaleFactor();
        }
    }


    template <class WarpFunc, class Weighter>
    inline void IntensityDepthKeyframe<WarpFunc, Weighter>::alignSingleScaleRobust( Result& result, const Image& gray, const Image& depth, size_t octave )
    {
        SIMD* simd = SIMD::instance();

        const typename Base::AlignmentData& kfdata = Base::_dataForScale[ octave ];

        T scale = ( T )depth.width() / ( T )gray.width();

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

        IMapScoped<const float> depthMap( depth );
        const float* dPtr = depthMap.ptr();
        size_t dStride = depthMap.stride() / sizeof( float );

        DepthJacType depthJacobian = DepthJacType::Zero();

        result.iterations = 0;
        while( result.iterations < Base::_maxIters ){
            // build the updated projection Matrix
            projMat = K4 * result.warp.poseMatrix();

            // project the points:
            simd->projectPoints( &warpedPts[ 0 ], projMat, &kfdata.points3d[ 0 ], kfdata.points3d.size() );
            simd->warpBilinear1f( &interpolatedPixels[ 0 ], &warpedPts[ 0 ].x, grayMap.ptr(), grayMap.stride(), width, height, -1.0f, num );

            deltaSum.setZero();
            hessian.setZero();

            result.numPixels = 0;
            result.costs = 0.0f;
            for( size_t i = 0; i < num; i++ ){
                if( interpolatedPixels[ i ] >= 0.0f ){
                    // compute the delta
                    float delta = result.warp.computeResidual( kfdata.pixelValues[ i ], interpolatedPixels[ i ] );

                    T weight = Base::_weighter.weight( delta );
                    jtmp = weight * kfdata.jacobians[ i ];

                    hessian.noalias() += jtmp.transpose() * kfdata.jacobians[ i ];
                    deltaSum += jtmp * delta;

                    // the depth values:
                    Vector2<T> p2d = scale * warpedPts[ i ];
                    float currDepth = dPtr[ ( int )p2d.y * dStride + ( int )p2d.x ] * Base::_depthScaling;
                    if( currDepth  > Base::_minDepth ){
                        evalDepthJacobian( depthJacobian, kfdata.points3d[ i ] );
                        float depthDelta = kfdata.points3d[ i ].z - currDepth;
                        // TODO: also use a robust estimator here
                        deltaSum += depthJacobian * depthDelta;
                    }

                    result.numPixels++;
                    result.costs += weight * Math::sqr( delta );
                }
            }

            if( !result.numPixels ){
                break;
            }

            // evaluate the delta parameters
            hessian.noalias() += kfdata.depthHessian;
            typename WarpFunc::DeltaVectorType deltaP = -hessian.inverse() * deltaSum.transpose();
            result.warp.updateParameters( deltaP );

            result.iterations++;
            if( deltaP.norm() < Base::_minUpdate )
                break;
        }
    }

    template <class WarpFunc, class Weighter>
    inline void IntensityDepthKeyframe<WarpFunc, Weighter>::alignSingleScaleNonRobust( Result& result, const Image& gray, const Image& depth, size_t octave )
    {
        SIMD* simd = SIMD::instance();
        Matrix4f projMat;

        const typename Base::AlignmentData& kfdata = Base::_dataForScale[ octave ];
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

        IMapScoped<const float> depthMap( depth );
        const float* dPtr = depthMap.ptr();
        size_t dStride = depthMap.stride() / sizeof( float );

        T scale = ( T )depth.width() / ( T )gray.width();
        DepthJacType depthJacobian = DepthJacType::Zero();

        result.iterations = 0;
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

                    // the depth values:
                    Vector2<T> p2d = scale * warpedPts[ i ];
                    float currDepth = dPtr[ ( int )p2d.y * dStride + ( int )p2d.x ] * Base::_depthScaling;
                    if( currDepth  > Base::_minDepth ){
                        evalDepthJacobian( depthJacobian, kfdata.points3d[ i ] );
                        float depthDelta = kfdata.points3d[ i ].z - currDepth;
                        deltaSum += depthJacobian * depthDelta;
                    }
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
    }
}

#endif // INTENSITYDEPTHKEYFRAME_H
