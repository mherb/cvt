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
                    _pixelPositions.push_back( Vector2f( x, y ) );
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
}
