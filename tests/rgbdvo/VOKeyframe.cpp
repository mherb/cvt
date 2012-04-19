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

        float gradThreshold = Math::sqr( 0.1f );

        size_t npts = 0;

        size_t ptIdx = 0;
        for( size_t y = 0; y < _depth.height(); y++ ){
            const float* gx = gxMap.ptr();
            const float* gy = gyMap.ptr();
            const float* value = grayMap.ptr();
            for( size_t x = 0; x < _depth.width(); x++, ptIdx++ ){
                const Vector3f& pcvt = points3d[ ptIdx ];

                if( pcvt.z < 0.01f ){
                    continue;
                } else {
                    EigenBridge::toEigen( p3d, pcvt );
                    g[ 0 ] = -gx[ x ];
                    g[ 1 ] = -gy[ x ];

                    if( g.squaredNorm() < gradThreshold )
                        continue;

                    pose.screenJacobian( J, p3d, K );

                    j = 0.5f * g.transpose() * J;

                    _jacobians.push_back( j );
                    _pixelValues.push_back( value[ x ] );
                    _points3d.push_back( pcvt );

                    H.noalias() += j.transpose() * j;
                    npts++;
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

        _gray.convolve( _gx, IKernel::HAAR_HORIZONTAL_3 );
        _gray.convolve( _gy, IKernel::HAAR_VERTICAL_3 );
    }

    void VOKeyframe::computeSDImages( const Matrix3f& intrinsics )
    {
        std::vector<size_t> strides( 6 );
        std::vector<float*> maps( 6 );
        std::vector<float*> sdPtrs( 6 );
        _steepestDescentImages.resize( 6 );

        for( size_t i = 0; i < 6; i++ ){
            _steepestDescentImages[ i ].reallocate( _gray.width(), _gray.height(), IFormat::GRAY_FLOAT );
            maps[ i ] = _steepestDescentImages[ i ].map<float>( &strides[ i ] );
            sdPtrs[ i ] = maps[ i ];
        }

        IMapScoped<const float> gxMap( _gx );
        IMapScoped<const float> gyMap( _gy );

        PointSet3f points3d;
        Vision::unprojectToXYZ( points3d, _depth, intrinsics, _depthScaling );

        SE3<float> pose; // dummy: TODO: the screenJacobian function of the pose can be static!

        Eigen::Vector2f g;
        SE3<float>::ScreenJacType J;
        J.setZero();
        JacType jacobian;
        HessianType hessian;
        hessian.setZero();

        Eigen::Matrix3f ke;
        Eigen::Vector3f pe;
        EigenBridge::toEigen( ke, intrinsics );

        size_t pidx = 0;
        for( size_t y = 0; y < _depth.height(); y++ ){
            const float* gx = gxMap.ptr();
            const float* gy = gyMap.ptr();
            for( size_t x = 0; x < _depth.width(); x++ ){
                const Vector3f& p = points3d[ pidx ];

                if( p.z > 0.05f ){
                    g[ 0 ] = -gx[ x ];
                    g[ 1 ] = -gy[ x ];
                    EigenBridge::toEigen( pe, p );
                    pose.screenJacobian( J, pe, ke );
                    jacobian = g.transpose() * J;
                    hessian.noalias() += jacobian.transpose() * jacobian;
                } else {
                    jacobian.setZero();
                }

                for( size_t i = 0; i < 6; i++ ){
                    sdPtrs[ i ][ x ] = jacobian[ i ];
                }

                pidx++;
            }

            /* update all the pointers */
            // next line for the sd images
            for( size_t i = 0; i < 6; i++ ){
                sdPtrs[ i ] += strides[ i ];
            }
            gxMap++;
            gyMap++;
        }

        HessianType hinv = hessian.inverse();

        //String file;
        for( size_t i = 0; i < 6; i++ ){
           _steepestDescentImages[ i ].unmap( maps[ i ] );

          // file.sprintf( "sdimage_%d.png", i );
          // _steepestDescentImages[ i ].save( file );
        }
        //_depth.save( "rgbdvo_depth.png" );
        //_gray.save( "rgbdvo_gray.png" );
    }
}
