#include <VOKeyframe.h>

#include <cvt/vision/Vision.h>
#include <cvt/gfx/IMapScoped.h>

#include <cvt/math/SE3.h>
#include <cvt/util/EigenBridge.h>

#include <Eigen/Dense>

namespace cvt
{
    VOKeyframe::VOKeyframe( const Image& rgb, const Image& depth, const Matrix4f& pose, const Matrix3f& K, float depthScaling ) :
        _pose( pose ),
        _depth( depth ),
        _depthScaling( 1.0f / depthScaling )
    {
        rgb.convert( _gray, IFormat::GRAY_FLOAT );
        computeGradients();
        computeJacobians( K );        
    }

    VOKeyframe::~VOKeyframe()
    {
    }

    void VOKeyframe::computeJacobians( const Matrix3f& intrinsics )
    {
        PointSet3f points3d;
        Vision::unprojectToXYZ( points3d, _depth, intrinsics, _depthScaling );

        IMapScoped<const float> gxMap( _gx );
        IMapScoped<const float> gyMap( _gy );
        IMapScoped<const float> grayMap( _gray );

        // eval the jacobians:
        Eigen::Vector3f p3d;
        Eigen::Vector2f g;
        Eigen::Matrix<float, 1, 6> j;
        SE3<float>::ScreenJacType J;

        HessianType H( HessianType::Zero() );

        Eigen::Matrix3f K;
        EigenBridge::toEigen( K, intrinsics );
        SE3<float> pose;

        size_t ptIdx = 0;
        for( size_t y = 0; y < _depth.height(); y++ ){
            const float* gx = gxMap.ptr();
            const float* gy = gyMap.ptr();
            const float* value = grayMap.ptr();
            for( size_t x = 0; x < _depth.width(); x++, ptIdx++ ){
                const Vector3f& pcvt = points3d[ ptIdx ];

                EigenBridge::toEigen( p3d, pcvt );

                if( p3d[ 2 ] < 0.1f ){
                    continue;
                } else {
                    g[ 0 ] = -gx[ x ];
                    g[ 1 ] = -gy[ x ];
                    pose.screenJacobian( J, p3d, K );

                    j = g.transpose() * J;

                    _jacobians.push_back( j );
                    _pixelValues.push_back( value[ x ] );
                    _points3d.push_back( pcvt );

                    H += j.transpose() * j;
                }
            }
            gxMap++;
            gyMap++;
            grayMap++;
        }

        // precompute the inverse hessian
        _inverseHessian = H.inverse();
    }

    void VOKeyframe::computeGradients()
    {
        _gx.reallocate( _gray.width(), _gray.height(), IFormat::GRAY_FLOAT);
        _gy.reallocate( _gray.width(), _gray.height(), IFormat::GRAY_FLOAT );

        _gray.convolve( _gx, IKernel::HAAR_HORIZONTAL_2 );
        _gray.convolve( _gy, IKernel::HAAR_VERTICAL_2 );
    }

    void VOKeyframe::computeSDImages( const Matrix3f& intrinsics )
    {
        std::vector<size_t> strides( 6 );
        std::vector<float*> maps( 6 );
        _steepestDescentImages.resize( 6 );

        for( size_t i = 0; i < 6; i++ ){
            _steepestDescentImages[ i ].reallocate( _gray.width(), _gray.height(), IFormat::GRAY_FLOAT );
            maps[ i ] = _steepestDescentImages[ i ].map<float>( &strides[ i ] );
        }

        PointSet3f points3d;
        Vision::unprojectToXYZ( points3d, _depth, intrinsics, _depthScaling );

        for( size_t y = 0; y < _depth.height(); y++ ){
            for( size_t x = 0; x < _depth.height(); x++ ){
                // TODO: fill the jacobian images
            }
        }

        for( size_t i = 0; i < 6; i++ ){
           _steepestDescentImages[ i ].unmap( maps[ i ] );
        }
    }
}
