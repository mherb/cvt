#include <VOKeyframe.h>

#include <cvt/gfx/IMapScoped.h>
#include <cvt/math/SE3.h>
#include <cvt/util/EigenBridge.h>

#include <Eigen/Dense>

namespace cvt
{
    VOKeyframe::VOKeyframe( const Image& gray, const Image& depth,
                            const Matrix4f& pose, const Matrix3f& K, float depthScaling ) :
        _pose( pose ),
        _gray( gray )
    {
        computeJacobians( depth, K, ( float )0xffff / depthScaling );
    }

    VOKeyframe::~VOKeyframe()
    {
    }

    void VOKeyframe::computeJacobians( const Image& depth, const Matrix3f& intrinsics, float depthScaling )
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
        Eigen::Matrix<float, 1, 6> j;
        SE3<float>::ScreenJacType J;

        HessianType H( HessianType::Zero() );

        Eigen::Matrix3f K;
        EigenBridge::toEigen( K, intrinsics );
        SE3<float> pose;

        float gradThreshold = Math::sqr( 0.1f );

        size_t npts = 0;
        for( size_t y = 0; y < depth.height(); y++ ){
            const float* gx = gxMap.ptr();
            const float* gy = gyMap.ptr();
            const float* value = grayMap.ptr();
            const float* d = depthMap.ptr();
            for( size_t x = 0; x < depth.width(); x++ ){
                float z = d[ x ] * depthScaling;
                if( z > 0.05f ){
                    g[ 0 ] = -gx[ x ];
                    g[ 1 ] = -gy[ x ];

                    if( g.squaredNorm() < gradThreshold )
                        continue;

                    p3d[ 0 ] = tmpx[ x ] * z;
                    p3d[ 1 ] = tmpy[ y ] * z;
                    p3d[ 2 ] = z;

                    pose.screenJacobian( J, p3d, K );

                    j = 0.5f * g.transpose() * J;

                    _jacobians.push_back( j );
                    _pixelValues.push_back( value[ x ] );
                    _points3d.push_back( Vector3f( p3d[ 0 ], p3d[ 1 ], p3d[ 2 ] ) );
                    _pixelPositions.push_back( Vector2f( x, y ) );

                    H.noalias() += j.transpose() * j;
                    npts++;
                }
            }
            gxMap++;
            gyMap++;
            grayMap++;
            depthMap++;
        }

        // precompute the inverse hessian
        _inverseHessian = H.inverse();

    }

    void VOKeyframe::computeGradients( Image& gx, Image& gy ) const
    {
        gx.reallocate( _gray.width(), _gray.height(), IFormat::GRAY_FLOAT);
        gy.reallocate( _gray.width(), _gray.height(), IFormat::GRAY_FLOAT );

        _gray.convolve( gx, IKernel::HAAR_HORIZONTAL_3 );
        _gray.convolve( gy, IKernel::HAAR_VERTICAL_3 );
    }
}
