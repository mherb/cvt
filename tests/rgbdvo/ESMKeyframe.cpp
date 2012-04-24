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
                            const Matrix4f& pose, const Matrix3f& K, float depthScaling ) :
        _pose( pose ),
        _gray( gray )
    {
        computeJacobians( depth, K, ( float )0xffff / depthScaling );
    }

    ESMKeyframe::~ESMKeyframe()
    {
    }

    void ESMKeyframe::computeJacobians( const Image& depth, const Matrix3f& intrinsics, float depthScaling )
    {
        Image gxI, gyI;
        computeGradients( gxI, gyI );

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

        float gradThreshold = Math::sqr( 0.1f );

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
                if( z > 0.05f ){
                    g[ 0 ] = -0.5f * gx[ x ];
                    g[ 1 ] = -0.5f * gy[ x ];

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
                    if( z > 0.05f ){
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
                    if( z > 0.05f ){
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
                    if( z > 0.05f ){
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
                    if( z > 0.05f ){
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

    void ESMKeyframe::computeGradients( Image& gx, Image& gy ) const
    {
        gx.reallocate( _gray.width(), _gray.height(), IFormat::GRAY_FLOAT);
        gy.reallocate( _gray.width(), _gray.height(), IFormat::GRAY_FLOAT );

        _gray.convolve( gx, IKernel::HAAR_HORIZONTAL_3 );
        _gray.convolve( gy, IKernel::HAAR_VERTICAL_3 );
    }


	ESMKeyframe::Result ESMKeyframe::computeRelativePose( SE3<float>& predicted,
														  const Image& gray,
														  const Matrix3f& intrinsics,
														  size_t maxIters ) const
	{
        Result result;
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

        while( result.iterations < maxIters ){
            // build the updated projection Matrix
            const Eigen::Matrix4f& m = predicted.transformation();
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
            predicted.applyInverse( -deltaP );

            result.iterations++;

            if( deltaP.norm() < 1e-4 ){
                return result;
            }
        }
        return result;
	}

    float ESMKeyframe::interpolatePixelValue( const Vector2f& pos, const float* ptr, size_t stride ) const
    {
        int lx = ( int )pos.x;
        int ly = ( int )pos.y;
        float fx = pos.x - lx;
        float fy = pos.y - ly;

        const float* p0 = ptr + ly * stride + lx;
        const float* p1 = p0 + stride;

        float v0 = Math::mix( p0[ 0 ], p0[ 1 ], fx );
        float v1 = Math::mix( p1[ 0 ], p1[ 1 ], fx );
        return Math::mix( v0, v1, fy );
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
