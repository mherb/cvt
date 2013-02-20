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
    template <class WarpFunc>
    class IntensityDepthKeyframe : public RGBDKeyframe<WarpFunc> {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            typedef RGBDKeyframe<WarpFunc>    Base;
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

                void reserve( size_t n )
                {
                    jacobians.reserve( n );
                }

                void clear()
                {
                    jacobians.clear();
                    lambda = 0.05f;
                }
            };

            std::vector<DepthData>  _depthDataForScale;

            bool checkResult( const Result& res, const Matrix4<T> &lastPose, size_t numPixels ) const;
            void alignSingleScaleNonRobust( Result& result, const Image& gray, const Image& depth, size_t octave );
            void alignSingleScaleRobust( Result& result, const Image& gray, const Image& depth, size_t octave );
    };

    template <class WarpFunc>
    inline IntensityDepthKeyframe<WarpFunc>::IntensityDepthKeyframe( const Mat3Type &K, size_t octaves, float scale ) :
        RGBDKeyframe<WarpFunc>( K, octaves, scale )
    {
        _depthDataForScale.resize( octaves );
    }

    template <class WarpFunc>
    inline IntensityDepthKeyframe<WarpFunc>::~IntensityDepthKeyframe()
    {
    }

    template <class WarpFunc>
    inline void IntensityDepthKeyframe<WarpFunc>::updateOfflineData( const Matrix4<T>& poseMat,
                                                                               const ImagePyramid& pyramid,
                                                                               const Image& depth )
    {
        this->_pose = poseMat;

        Image gxI, gyI;
        float scale = 1.0f;

        Vector3<T> p3d;
        Eigen::Matrix<T, 2, 1> g, depthGrad;
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

            // temp vals
            std::vector<float> tmpx( gray.width() );
            std::vector<float> tmpy( gray.height() );
            Base::initializePointLookUps( &tmpx[ 0 ], tmpx.size(), data.intrinsics[ 0 ][ 0 ], data.intrinsics[ 0 ][ 2 ] );
            Base::initializePointLookUps( &tmpy[ 0 ], tmpy.size(), data.intrinsics[ 1 ][ 1 ], data.intrinsics[ 1 ][ 2 ] );

            IMapScoped<const float> gxMap( gxI );
            IMapScoped<const float> gyMap( gyI );
            IMapScoped<const float> grayMap( gray );

            std::vector<float> depthLastY( gray.width() );

            data.hessian.setZero();
            depthData.hessian.setZero();
            for( size_t y = 0; y < gray.height(); y++ ){
                currP.y = scale * y;
                const float* gx = gxMap.ptr();
                const float* gy = gyMap.ptr();
                const float* value = grayMap.ptr();

                float lastZ = 0.0f;
                for( size_t x = 0; x < gray.width(); x++ ){
                    currP.x = scale * x;
                    float z = Base::interpolateDepth( currP, dPtr, depthStride );
                    if( z > this->_minDepth ){
                        g[ 0 ] = gx[ x ];
                        g[ 1 ] = gy[ x ];

                        depthGrad.setZero();

                        if( lastZ > this->_minDepth )
                            depthGrad[ 0 ] = z - lastZ;
                        if( depthLastY[ x ] > this->_minDepth )
                            depthGrad[ 1 ] = z - depthLastY[ x ];

                        float salience = Math::abs( g[ 0 ] ) + Math::abs( g[ 1 ] );
                        salience += depthData.lambda * ( Math::abs( depthGrad[ 0 ] ) + Math::abs( depthGrad[ 1 ] ) );

                        if( salience > this->_gradientThreshold ){
                            p3d[ 0 ] = tmpx[ x ] * z;
                            p3d[ 1 ] = tmpy[ y ] * z;
                            p3d[ 2 ] = z;

                            WarpFunc::computeJacobian( j, p3d, data.intrinsics, g, value[ x ] );
                            data.jacobians.push_back( j );
                            data.pixelValues.push_back( value[ x ] );
                            data.points3d.push_back( p3d );
                            data.hessian.noalias() += j.transpose() * j;

                            WarpFunc::computeJacobian( dJac, p3d, data.intrinsics, depthGrad, z );
                            dJac *= depthData.lambda;

                            depthData.hessian.noalias() += dJac.transpose() * dJac;
                            depthData.jacobians.push_back( dJac );
                        }
                    }
                    lastZ = z;
                    depthLastY[ x ] = z;
                }
                gxMap++;
                gyMap++;
                grayMap++;
            }

            // precompute the inverse hessian
            data.inverseHessian = ( data.hessian + depthData.hessian ).inverse();

            scale /= pyramid.scaleFactor();
        }
    }


    template <class WarpFunc>
    inline void IntensityDepthKeyframe<WarpFunc>::alignSingleScaleRobust( Result& result, const Image& gray, const Image& depth, size_t octave )
    {
        SIMD* simd = SIMD::instance();

        const typename Base::AlignmentData& kfdata = Base::_dataForScale[ octave ];
        const DepthData& depthData = _depthDataForScale[ octave ];

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

        result.iterations = 0;
        result.pixelPercentage = 0;
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

                    result.numPixels++;
                    result.costs += weight * Math::sqr( delta );

                    // the depth values:
                    Vector2<T> p2d = scale * warpedPts[ i ];
                    float currZ = Base::interpolateDepth( p2d, dPtr, dStride );
                    if( currZ  > Base::_minDepth ){
                        float depthDelta = kfdata.points3d[ i ].z - currZ;
                        // TODO: also use a robust estimator here
                        deltaSum += depthData.jacobians[ i ] * depthDelta;
                    }
                }
            }

            if( !result.numPixels ){
                break;
            }

            // evaluate the delta parameters
            hessian.noalias() += depthData.hessian;
            typename WarpFunc::DeltaVectorType deltaP = -hessian.inverse() * deltaSum.transpose();
            result.warp.updateParameters( deltaP );

            result.iterations++;
            if( deltaP.norm() < Base::_minUpdate )
                break;
        }

        result.pixelPercentage = result.numPixels / num;
    }

    template <class WarpFunc>
    inline void IntensityDepthKeyframe<WarpFunc>::alignSingleScaleNonRobust( Result& result, const Image& gray, const Image& depth, size_t octave )
    {
        SIMD* simd = SIMD::instance();
        Matrix4f projMat;

        const typename Base::AlignmentData& kfdata = Base::_dataForScale[ octave ];
        const DepthData& depthData = _depthDataForScale[ octave ];

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

        result.iterations = 0;
        result.numPixels = 0;
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
                    float currZ = Base::interpolateDepth( p2d, dPtr, dStride );
                    if( currZ  > Base::_minDepth ){
                        float depthDelta = kfdata.points3d[ i ].z - currZ;
                        deltaSum += depthData.jacobians[ i ] * depthDelta;
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
        result.pixelPercentage = result.numPixels / num;
    }
}

#endif // INTENSITYDEPTHKEYFRAME_H
