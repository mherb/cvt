#include <AIIKeyframe.h>

#include <cvt/gfx/IMapScoped.h>
#include <cvt/math/SE3.h>
#include <cvt/util/EigenBridge.h>

#include <Eigen/Dense>

namespace cvt
{
    AIIKeyframe::AIIKeyframe( const Image& gray, const Image& depth,
                            const Matrix4f& pose, const Matrix3f& K, const VOParams& params ) :
        _pose( pose ),
        _gray( gray )
    {
        computeJacobians( depth, K, params );
    }

    AIIKeyframe::~AIIKeyframe()
    {        
    }

    void AIIKeyframe::computeJacobians( const Image& depth, const Matrix3f& intrinsics, const VOParams& params )
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
        JacType j;
        SE3<float>::ScreenJacType J;

        HessianType H( HessianType::Zero() );

        Eigen::Matrix3f K;
        EigenBridge::toEigen( K, intrinsics );
        SE3<float> pose;

        float gradThreshold = Math::sqr( params.gradientThreshold );

        for( size_t y = 0; y < depth.height(); y++ ){
            const float* gx = gxMap.ptr();
            const float* gy = gyMap.ptr();
            const float* value = grayMap.ptr();
            const float* d = depthMap.ptr();
            for( size_t x = 0; x < depth.width(); x++ ){
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

                    j.head<6>() = g.transpose() * J;
                    j[ 6 ] = value[ x ];
                    j[ 7 ] = 1.0f;

                    _jacobians.push_back( j );
                    _pixelValues.push_back( value[ x ] );
                    _points3d.push_back( Vector3f( p3d[ 0 ], p3d[ 1 ], p3d[ 2 ] ) );                    
                    H.noalias() += j.transpose() * j;                    
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

    VOResult AIIKeyframe::computeRelativePose( PoseRepresentation& predicted,
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
        JacType deltaSum, jtmp;

        IMapScoped<const float> grayMap( gray );
        size_t floatStride = grayMap.stride() / sizeof( float );

        while( result.iterations < params.maxIters ){
            // build the updated projection Matrix
            const Eigen::Matrix4f& m = predicted.pose.transformation();
            mEigen.block<3, 3>( 0, 0 ) = Keigen * m.block<3, 3>( 0, 0 );
            mEigen.block<3, 1>( 0, 3 ) = Keigen * m.block<3, 1>( 0, 3 );
            EigenBridge::toCVT( projMat, mEigen );

            // project the points:
            simd->projectPoints( &warpedPts[ 0 ], projMat, &_points3d[ 0 ], _points3d.size() );

            deltaSum.setZero();
            result.numPixels = 0;
            result.SSD = 0.0f;

            for( size_t i = 0; i < warpedPts.size(); i++ ){
                const Vector2f& pw = warpedPts[ i ];
                if( pw.x > 0.0f && pw.x < ( gray.width()  - 1 ) &&
                    pw.y > 0.0f && pw.y < ( gray.height() - 1 ) ){
                    float v = interpolatePixelValue( pw, grayMap.ptr(), floatStride );

                    // bias gain:
                    v = ( 1.0f + predicted.gain ) * v + predicted.bias;

                    // compute the delta
                    float delta = _pixelValues[ i ] - v;
                    result.SSD += Math::sqr( delta );
                    result.numPixels++;

                    jtmp = delta * _jacobians[ i ];
                    deltaSum += jtmp;
                }
            }

            // evaluate the delta parameters
            //SE3<float>::ParameterVectorType deltaP = -_inverseHessian * deltaSum.transpose();
            Eigen::Matrix<float, 8, 1> deltaP = -_inverseHessian * deltaSum.transpose();

            predicted.pose.applyInverse( -deltaP.head<6>() );
            // update bias and gain
            predicted.bias = ( predicted.bias - deltaP[ 7 ] ) / ( 1.0f + deltaP[ 6 ] );
            predicted.gain = ( predicted.gain - deltaP[ 6 ] ) / ( 1.0f + deltaP[ 6 ] );
//            predicted.bias = predicted.bias - ( predicted.gain  + 1.0f ) * deltaP[ 7 ] ;
//            predicted.gain = ( predicted.gain + 1.0f ) / ( 1.0f + deltaP[ 6 ] );

            result.iterations++;
            if( deltaP.norm() < params.minParameterUpdate )
                return result;
        }
        return result;
    }
}
