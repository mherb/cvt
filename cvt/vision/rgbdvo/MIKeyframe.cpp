#include <cvt/vision/rgbdvo/MIKeyframe.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/math/SE3.h>
#include <cvt/math/BSpline.h>
#include <cvt/math/Vector.h>
#include <cvt/util/EigenBridge.h>

#include <Eigen/Dense>

namespace cvt
{
    MIKeyframe::MIKeyframe( const Image& gray, const Image& depth,
                            const Matrix4f& pose, const Matrix3f& K, const VOParams& params ) :
        _pose( pose ),
        _gray( gray ),
		_numBins( 10 )
    {
		_jointHistogram		= new float[ Math::sqr( _numBins + 1 ) ];
		_templateHistogram	= new float[ _numBins ];

        computeJacobians( depth, K, params );
    }

    MIKeyframe::~MIKeyframe()
    {        
    }

    void MIKeyframe::computeJacobians( const Image& depth, const Matrix3f& intrinsics, const VOParams& params )
    {
        Image gxI, gyI, gxxI, gyyI, gxyI;

		gxI.reallocate( _gray );
		gyI.reallocate( _gray );
		gxxI.reallocate( _gray );
		gyyI.reallocate( _gray );
		gxyI.reallocate( _gray );

		IKernel kx = IKernel::HAAR_HORIZONTAL_3;
		IKernel ky = IKernel::HAAR_HORIZONTAL_3;
		kx.scale( -0.5f );
		ky.scale( -0.5f );

		_gray.convolve( gxI, kx );
		_gray.convolve( gyI, ky );
		_gray.convolve( gxyI, kx, ky );
		_gray.convolve( gxxI, IKernel::LAPLACE_3_XX );
		_gray.convolve( gyyI, IKernel::LAPLACE_3_YY );

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
        IMapScoped<const float> gxxMap( gxxI );
        IMapScoped<const float> gyyMap( gyyI );
        IMapScoped<const float> gxyMap( gxyI );
        IMapScoped<const float> grayMap( _gray );
        IMapScoped<const float> depthMap( depth );

        // eval the jacobians:
        Eigen::Vector3f p3d;
        Eigen::Vector2f g;
        Eigen::Matrix2f hess;

        Eigen::Matrix<float, 1, 6> j;
        SE3<float>::ScreenJacType J;
        SE3<float>::ScreenHessType wx, wy;

        HessianType hessian;

        Eigen::Matrix3f K;
        EigenBridge::toEigen( K, intrinsics );
        SE3<float> pose;

        float gradThreshold = Math::sqr( params.gradientThreshold );

        for( size_t y = 0; y < depth.height(); y++ ){
            const float* gx  = gxMap.ptr();
            const float* gy  = gyMap.ptr();
            const float* gxx = gxxMap.ptr();
            const float* gyy = gyyMap.ptr();
            const float* gxy = gxyMap.ptr();

            const float* value = grayMap.ptr();
            const float* d = depthMap.ptr();

            float normFactor = ( float ) ( _numBins - 3 );
            float pixVal;

            for( size_t x = 0; x < depth.width(); x++ ){
                float z = d[ x ] * depthScaling;
                if( z > params.minDepth ){
                    g[ 0 ] = gx[ x ];
                    g[ 1 ] = gy[ x ];

                    if( g.squaredNorm() < gradThreshold )
                        continue;
					
                    hess( 0, 0 ) = gxx[ x ]; // TODO: maybe negate this??
					hess( 0, 1 ) = gxy[ x ];
					hess( 1, 0 ) = gxy[ x ];
                    hess( 1, 1 ) = gyy[ x ]; // TODO: maybe negate this??

					g	 *= ( float )( _numBins - 3 );
					hess *= ( float )( _numBins - 3 ); // TODO: really?
				
                    p3d[ 0 ] = tmpx[ x ] * z;
                    p3d[ 1 ] = tmpy[ y ] * z;
                    p3d[ 2 ] = z;

                    pose.screenJacobian( J, p3d, K );
					pose.screenHessian( wx, wy, p3d, K );

                    j = g.transpose() * J;

                    hessian = J.transpose() * hess * J + g[ 0 ] * wx + g[ 1 ] * wy;

                    _jacobians.push_back( j );                    

                    _jacobiansOuterProduct.push_back( j.transpose() * j );

                    _hessians.push_back( hessian );

                    _pixelValues.push_back( value[ x ] );

                    _points3d.push_back( Vector3f( p3d[ 0 ], p3d[ 1 ], p3d[ 2 ] ) );

                    pixVal = normFactor * value[ x ] + 1.0f;
                    int binIdx = ( int ) pixVal;
                    //_binValues.push_back( binIdx );
                    Vector4f weights, derivWeights;
					for( int bin = 0; bin < 4; bin++ ){
                        weights[ bin ]      = BSpline<float>::evalDerivative( (float)( binIdx + bin - 1 ) - pixVal );
                        derivWeights[ bin ] = BSpline<float>::evalSecondDerivative( (float)( binIdx + bin - 1 ) - pixVal );
					}
                    _splineWeights.push_back( weights );
                    _splineDerivativeWeights.push_back( derivWeights );
                }
            }
            gxMap++;            
            gyMap++;
            gxxMap++;
            gyyMap++;
            gxyMap++;
            grayMap++;
            depthMap++;
        }
    }

    VOResult MIKeyframe::computeRelativePose( PoseRepresentation& predicted,
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

        Eigen::Matrix4f mEigen( Eigen::Matrix4f::Identity() );

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
            //predicted.pose.applyInverse( -deltaP );

            result.iterations++;
            //if( deltaP.norm() < params.minParameterUpdate )
            //    return result;
        }
        return result;
    }
}
