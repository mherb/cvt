/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
*/

#include <ESMAlignment.h>
#include <cvt/util/EigenBridge.h>
#include <cvt/gfx/IMapScoped.h>

namespace cvt
{
    ESMAlignment::ESMAlignment( const Matrix3f& K, size_t maxIterations, float depthScale ) :
        _intrinsics( K ),
        _maxIterations( maxIterations ),
        _depthScale( ( float )0xffff / depthScale )
    {
    }

    ESMAlignment::~ESMAlignment()
    {
    }

    ESMAlignment::Result ESMAlignment::alignWithKeyframe( SE3<float>& predicted,
                                                          const ESMKeyframe& keyframe,
                                                          const Image& gray )
    {
        Result result;
        result.SSD = 0.0f;
        result.iterations = 0;
        result.numPixels = 0;

        SIMD* simd = SIMD::instance();
        Matrix4f projMat;

        std::vector<Vector2f> warpedPts;
        warpedPts.resize( keyframe.numPoints() * 5 );

        Eigen::Matrix4f mEigen;
        mEigen.setIdentity();

        Eigen::Matrix3f Keigen;
        EigenBridge::toEigen( Keigen, _intrinsics );


        // sum of jacobians * delta
        ESMKeyframe::JacType jac;
        ESMKeyframe::HessianType approxHessian;
        Eigen::Matrix<float, 1, 6> deltaSum, jtmp;
        Eigen::Matrix<float, 1, 2> gradient;
        Vector2f warpedGrad;

        IMapScoped<const float> grayMap( gray );
        const float* kfPixels = keyframe.pixelData();
        const Vector2f* tempGradients = keyframe.gradients();
        const ESMKeyframe::JacType* jacobians = keyframe.jacobians();

        size_t floatStride = grayMap.stride() / sizeof( float );

        Image warped( gray.width(), gray.height(), IFormat::GRAYALPHA_FLOAT );

        size_t numPositions = keyframe.numPoints();

        while( result.iterations < _maxIterations ){
            // build the updated projection Matrix
            const Eigen::Matrix4f& m = predicted.transformation();
            mEigen.block<3, 3>( 0, 0 ) = Keigen * m.block<3, 3>( 0, 0 );
            mEigen.block<3, 1>( 0, 3 ) = Keigen * m.block<3, 1>( 0, 3 );
            EigenBridge::toCVT( projMat, mEigen );

            // project the points:
            simd->projectPoints( &warpedPts[ 0 ], projMat, keyframe.pointsPtr(), numPositions * 5 );

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
                    float delta = kfPixels[ i ] - v;
                    result.SSD += Math::sqr( delta );
                    result.numPixels++;

                    gradient[ 0 ] = tempGradients[ i ].x;
                    gradient[ 1 ] = tempGradients[ i ].y;

                    // if we can evaluate a warped gradient, we do esm style, else normal!
                    if( evalGradient( warpedGrad, &warpedPts[ i * 5 ], grayMap.ptr(), floatStride, gray.width(), gray.height() ) ){
                        gradient[ 0 ] += warpedGrad.x;
                        gradient[ 1 ] += warpedGrad.y;
                        gradient *= 0.5f;
                    }

                    jtmp = gradient * jacobians[ i ];
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

    float ESMAlignment::interpolatePixelValue( const Vector2f& pos, const float* ptr, size_t stride )
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

    bool ESMAlignment::evalGradient( Vector2f& grad, const Vector2f* positions, const float* ptr, size_t stride, size_t w, size_t h )
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
