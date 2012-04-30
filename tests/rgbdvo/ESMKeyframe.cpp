/*
            CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
*/

#include <ESMKeyframe.h>

#include <cvt/gfx/IMapScoped.h>
#include <cvt/math/SE3.h>
#include <cvt/util/EigenBridge.h>

#include <Eigen/Dense>

namespace cvt
{
    ESMKeyframe::ESMKeyframe( const Image& gray, const Image& depth,
                             const Matrix4f& pose, const Matrix3f& K, const VOParams& params ) :
        _pose( pose ),
        _gray( gray )
    {
        computeJacobians( depth, K, params );
    }

    ESMKeyframe::~ESMKeyframe()
    {
    }

    void ESMKeyframe::computeJacobians( const Image& depth, const Matrix3f& intrinsics, const VOParams& params )
    {
        Image gxI, gyI;
        computeGradients( gxI, gyI, _gray );

        float depthScaling = ( float )0xffff / params.depthScale;

        float invFx = 1.0f / intrinsics[ 0 ][ 0 ];
        float invFy = 1.0f / intrinsics[ 1 ][ 1 ];
        float cx    = intrinsics[ 0 ][ 2 ];
        float cy    = intrinsics[ 1 ][ 2 ];

        // temp vals
        std::vector<float> tmpx( depth.width() );
        std::vector<float> tmpy( depth.height() );

        for( size_t i = 0; i < tmpx.size(); i++ ){
            tmpx[ i ] = ( i - cx ) * invFx;
        }
        for( size_t i = 0; i < tmpy.size(); i++ ){
            tmpy[ i ] = ( i - cy ) * invFy;
        }

        IMapScoped<const float> gxMap( gxI );
        IMapScoped<const float> gyMap( gyI );
        IMapScoped<const float> grayMap( _gray );
        IMapScoped<const float> depthMap( depth );

        // eval the jacobians:
        Eigen::Vector3f p3d;
        Eigen::Vector2f g;

        SE3<float>::ScreenJacType J;

        Eigen::Matrix3f K;
        EigenBridge::toEigen( K, intrinsics );
        SE3<float> pose;

        float gradThreshold = Math::sqr( params.gradientThreshold );

        gxMap++;
        gyMap++;
        grayMap++;
        depthMap++;
        for( size_t y = 1; y < depth.height() - 1; y++ ){
            const float* gx = gxMap.ptr();
            const float* gy = gyMap.ptr();
            const float* value = grayMap.ptr();
            const float* d = depthMap.ptr();
            for( size_t x = 1; x < depth.width() - 1; x++ ){
                float z = d[ x ] * depthScaling;
                if( z > params.minDepth ){
                    g[ 0 ] = gx[ x ];
                    g[ 1 ] = gy[ x ];

                    if( g.squaredNorm() < gradThreshold )
                        continue;

                    p3d[ 0 ] = tmpx[ x ] * z;
                    p3d[ 1 ] = tmpy[ y ] * z;
                    p3d[ 2 ] = z;

                    pose.screenJacobian( J, p3d, K );

                    _jacobians.push_back( J );
                    _pixelValues.push_back( value[ x ] );
                    _templateGradients.push_back( Vector2f( g[ 0 ], g[ 1 ] ) );
                    _points3d.push_back( Vector3f( p3d[ 0 ], p3d[ 1 ], p3d[ 2 ] ) );

                    // now check the neigbours -> needed for esm gradient
                    float centerZ = z;
                    z = d[ x - 1 ] * depthScaling;
                    if( z > params.minDepth ){
                        p3d[ 0 ] = tmpx[ x - 1 ] * z;
                        p3d[ 1 ] = tmpy[ y ] * z;
                        p3d[ 2 ] = z;
                    } else {
                        p3d[ 0 ] = tmpx[ x - 1 ] * centerZ;
                        //p3d[ 1 ] = tmpy[ y ] * centerZ;
                        //p3d[ 2 ] = centerZ;
                    }
                    _points3d.push_back( Vector3f( p3d[ 0 ], p3d[ 1 ], p3d[ 2 ] ) );

                    // now check the neigbours -> needed for esm gradient
                    z = d[ x + 1 ] * depthScaling;
                    if( z > params.minDepth ){
                        p3d[ 0 ] = tmpx[ x + 1 ] * z;
                        p3d[ 1 ] = tmpy[ y ] * z;
                        p3d[ 2 ] = z;
                    } else {
                        p3d[ 0 ] = tmpx[ x + 1 ] * centerZ;
                        p3d[ 1 ] = tmpy[ y ] * centerZ;
                        p3d[ 2 ] = centerZ;
                    }
                    _points3d.push_back( Vector3f( p3d[ 0 ], p3d[ 1 ], p3d[ 2 ] ) );

                    depthMap--;
                    z = d[ x ] * depthScaling;
                    if( z > params.minDepth ){
                        p3d[ 0 ] = tmpx[ x ] * z;
                        p3d[ 1 ] = tmpy[ y - 1 ] * z;
                        p3d[ 2 ] = z;
                    } else {
                        p3d[ 0 ] = tmpx[ x ] * centerZ;
                        p3d[ 1 ] = tmpy[ y - 1 ] * centerZ;
                        p3d[ 2 ] = centerZ;
                    }
                    _points3d.push_back( Vector3f( p3d[ 0 ], p3d[ 1 ], p3d[ 2 ] ) );

                    depthMap++; // back to current line
                    depthMap++; // next line
                    z = d[ x ] * depthScaling;
                    if( z > params.minDepth ){
                        p3d[ 0 ] = tmpx[ x ] * z;
                        p3d[ 1 ] = tmpy[ y + 1 ] * z;
                        p3d[ 2 ] = z;
                    } else {
                        p3d[ 0 ] = tmpx[ x ] * centerZ;
                        p3d[ 1 ] = tmpy[ y + 1 ] * centerZ;
                        p3d[ 2 ] = centerZ;
                    }
                    _points3d.push_back( Vector3f( p3d[ 0 ], p3d[ 1 ], p3d[ 2 ] ) );
                    depthMap--; // back to current line
                }
            }
            gxMap++;
            gyMap++;
            grayMap++;
            depthMap++;
        }
    }

    VOResult ESMKeyframe::computeRelativePose( PoseRepresentation& predicted,
                                               const Image& gray,
                                               const Matrix3f& intrinsics,
                                               const VOParams& params ) const
	{
        VOResult result;
        result.SSD = 0.0f;
        result.iterations = 0;
        result.numPixels = 0;

        SIMD* simd = SIMD::instance();
        Matrix4f projMat;

        std::vector<Vector2f> warpedPts;
        warpedPts.resize( _points3d.size() );

        Eigen::Matrix4f mEigen;
        mEigen.setIdentity();

        Eigen::Matrix3f Keigen;
        EigenBridge::toEigen( Keigen, intrinsics );


        // sum of jacobians * delta
        ESMKeyframe::JacType jac;
        ESMKeyframe::HessianType approxHessian;
        Eigen::Matrix<float, 1, 6> deltaSum, jtmp;
        Eigen::Matrix<float, 1, 2> gradient;
        Vector2f warpedGrad;

        IMapScoped<const float> grayMap( gray );

        size_t floatStride = grayMap.stride() / sizeof( float );
        size_t numPositions = _pixelValues.size();

        while( result.iterations < params.maxIters ){
            // build the updated projection Matrix
            const Eigen::Matrix4f& m = predicted.pose.transformation();
            mEigen.block<3, 3>( 0, 0 ) = Keigen * m.block<3, 3>( 0, 0 );
            mEigen.block<3, 1>( 0, 3 ) = Keigen * m.block<3, 1>( 0, 3 );
            EigenBridge::toCVT( projMat, mEigen );

            // project the points:
            simd->projectPoints( &warpedPts[ 0 ], projMat, &_points3d[ 0 ], _points3d.size() );

            deltaSum.setZero();
            approxHessian.setZero();
            result.numPixels = 0;
            result.SSD = 0.0f;

            for( size_t i = 0; i < numPositions; i++ ){
                const Vector2f& pw = warpedPts[ i * 5 ];
                if( pw.x > 0.0f && pw.x < ( gray.width()  - 1 ) &&
                    pw.y > 0.0f && pw.y < ( gray.height() - 1 ) ){
                    float v = interpolatePixelValue( pw, grayMap.ptr(), floatStride );

                    // compute the delta
                    float delta = _pixelValues[ i ] - v;
                    result.SSD += Math::sqr( delta );
                    result.numPixels++;

                    gradient[ 0 ] = _templateGradients[ i ].x;
                    gradient[ 1 ] = _templateGradients[ i ].y;

                    // if we can evaluate a warped gradient, we do esm style, else normal!
                    if( evalGradient( warpedGrad, &warpedPts[ i * 5 ], grayMap.ptr(), floatStride, gray.width(), gray.height() ) ){
                        gradient[ 0 ] += warpedGrad.x;
                        gradient[ 1 ] += warpedGrad.y;
                        gradient *= 0.5f;
                    }

                    jtmp = gradient * _jacobians[ i ];
                    deltaSum += delta * jtmp;
                    approxHessian.noalias() += jtmp.transpose() * jtmp;
                }
            }

            // evaluate the delta parameters
            SE3<float>::ParameterVectorType deltaP = -approxHessian.inverse() * deltaSum.transpose();
            predicted.pose.applyInverse( -deltaP );

            result.iterations++;

            if( deltaP.norm() < params.minParameterUpdate ){
                return result;
            }
        }
        return result;
	}

    bool ESMKeyframe::evalGradient( Vector2f& grad, const Vector2f* positions, const float* ptr, size_t stride, size_t w, size_t h ) const
    {
        if( positions[ 0 ].x < 0 || positions[ 0 ].x > w - 1 || positions[ 0 ].y < 0 || positions[ 0 ].y > h - 1 ||
            positions[ 1 ].x < 0 || positions[ 1 ].x > w - 1 || positions[ 1 ].y < 0 || positions[ 1 ].y > h - 1 ||
            positions[ 2 ].x < 0 || positions[ 2 ].x > w - 1 || positions[ 2 ].y < 0 || positions[ 2 ].y > h - 1 ||
            positions[ 3 ].x < 0 || positions[ 3 ].x > w - 1 || positions[ 3 ].y < 0 || positions[ 3 ].y > h - 1 )
            return false;
        float v0, v1;
        v0 = interpolatePixelValue( positions[ 0 ], ptr, stride );
        v1 = interpolatePixelValue( positions[ 1 ], ptr, stride );
        grad.x = 0.5f * ( v1 - v0 );
        v0 = interpolatePixelValue( positions[ 2 ], ptr, stride );
        v1 = interpolatePixelValue( positions[ 3 ], ptr, stride );
        grad.y = 0.5f * ( v1 - v0 );
        return true;
    }
}
