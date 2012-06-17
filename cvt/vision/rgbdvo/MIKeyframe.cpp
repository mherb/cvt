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
        _kx( IKernel::HAAR_HORIZONTAL_3 ),
        _ky( IKernel::HAAR_VERTICAL_3 ),
        _kxx( IKernel::LAPLACE_5_XX ),
        _kyy( IKernel::LAPLACE_5_YY ),
        _pose( pose ),
        _gray( gray ),
		_numBins( 10 )
    {
        _kx.scale( -0.5f );
        _ky.scale( -0.5f );
        _kxx.scale( -0.25f );
        _kyy.scale( -0.25f );

		_jointHistogram		= new float[ Math::sqr( _numBins + 1 ) ];
        _templateHistogram	= new float[ _numBins + 1 ];
        computeJacobians( depth, K, params );
    }

    MIKeyframe::~MIKeyframe()
    {        
        if( _jointHistogram )
            delete[] _jointHistogram;

        if( _templateHistogram )
            delete[] _templateHistogram;
    }

    void MIKeyframe::computeJacobians( const Image& depth, const Matrix3f& intrinsics, const VOParams& params )
    {
        Image gxI, gyI, gxxI, gyyI, gxyI;

		gxI.reallocate( _gray );
		gyI.reallocate( _gray );
		gxxI.reallocate( _gray );
		gyyI.reallocate( _gray );
		gxyI.reallocate( _gray );

        _gray.convolve( gxI, _kx );
        _gray.convolve( gyI, _ky );
        _gray.convolve( gxyI, _kx, _ky );
        _gray.convolve( gxxI, _kxx );
        _gray.convolve( gyyI, _kyy );

        float depthScaling = ( float )0xffff / params.depthScale;

        float invFx = 1.0f / intrinsics[ 0 ][ 0 ];
        float invFy = 1.0f / intrinsics[ 1 ][ 1 ];
        float cx    = intrinsics[ 0 ][ 2 ];
        float cy    = intrinsics[ 1 ][ 2 ];

        // temp vals for speeding up the unprojection
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

        // we also compute the histograms on the fly, so zero them here!:
        memset( _templateHistogram, 0, (_numBins + 1)*sizeof(float) );
        memset( _jointHistogram, 0, Math::sqr(_numBins + 1)*sizeof(float) );

        float normFactor = ( float ) ( _numBins - 3 );
        for( size_t y = 0; y < depth.height(); y++ ){
            const float* gx  = gxMap.ptr();
            const float* gy  = gyMap.ptr();
            const float* gxx = gxxMap.ptr();
            const float* gyy = gyyMap.ptr();
            const float* gxy = gxyMap.ptr();

            const float* value = grayMap.ptr();
            const float* d = depthMap.ptr();

            float pixVal;
            for( size_t x = 0; x < depth.width(); x++ ){
                float z = d[ x ] * depthScaling;
                if( z > params.minDepth ){
                    g[ 0 ] = gx[ x ];
                    g[ 1 ] = gy[ x ];                    
					
                    hess( 0, 0 ) = gxx[ x ]; hess( 0, 1 ) = gxy[ x ];
                    hess( 1, 0 ) = gxy[ x ]; hess( 1, 1 ) = gyy[ x ];

                    g	 *= ( float )( _numBins - 3 ); // scale to the number of bins
                    hess *= ( float )( _numBins - 3 ); // scale to the number of bins
				
                    p3d[ 0 ] = tmpx[ x ] * z;
                    p3d[ 1 ] = tmpy[ y ] * z;
                    p3d[ 2 ] = z;

                    pose.screenJacobian( J, p3d, K );
					pose.screenHessian( wx, wy, p3d, K );

                    j = g.transpose() * J;

                    hessian = J.transpose() * hess * J + g[ 0 ] * wx + g[ 1 ] * wy;

                    _jacobians.push_back( j );
                    _hessians.push_back( hessian );

                    _pixelValues.push_back( value[ x ] );

                    _points3d.push_back( Vector3f( p3d[ 0 ], p3d[ 1 ], p3d[ 2 ] ) );

                    pixVal = normFactor * value[ x ] + 1.0f;

                    float binIdx = ( int ) pixVal - 1 - pixVal;
                    Vector4f dWeights, d2Weights, weights;
                    for( int bin = 0; bin < 4; bin++ ){
                        float idx = binIdx + bin;
                        weights[ bin ]   = BSpline<float>::eval( idx );
                        dWeights[ bin ]  = BSpline<float>::evalDerivative( idx );
                        d2Weights[ bin ] = BSpline<float>::evalSecondDerivative( idx );
					}

                    _splineWeights.push_back( weights );
                    _splineDerivativeWeights.push_back( dWeights );
                    _splineSecondDerivativeWeights.push_back( d2Weights );

                    // update the template histogram and joint histogram values
                    addToHistograms( ( int )pixVal, weights );
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

        // normalize the histograms by the number of points
        float norm = 1.0f / _pixelValues.size();
        SIMD* simd = SIMD::instance();
        simd->MulValue1f( _templateHistogram, _templateHistogram, norm, _numBins + 1 );
        simd->MulValue1f( _jointHistogram, _jointHistogram, norm, Math::sqr( _numBins + 1 ) );

        // compute the template hessian from the stored quantities
        evaluateHessian();
    }

    void MIKeyframe::addToHistograms( int bin, const Vector4f& weights )
    {
        // is the bin
        Vector4f tmp;
        for( int t = bin - 1, i = 0; t < 3; t++, i++ ){
            _templateHistogram[ t ] += weights[ i ];
            // the joint histogram:
            float* jrow = _jointHistogram + t * ( _numBins + 3 );

            tmp = weights[ i ] * weights;
            for( int r = bin - 1, k = 0; r < 3; r++, k++ )
                jrow[ r ] += tmp[ k ];
        }
    }

    void MIKeyframe::evaluateHessian()
    {
        /*
        JacType* jacobianHistogram = new JacType[ ( _numBins + 1 ) * ( _numBins + 1 ) ];

        for( size_t y = 0; y < _numBins+1; y++ ) {
            for( size_t x = 0; x < _numBins+1; x++ ) {
                jacobianHistogram[ y * ( _numBins + 1 ) + x ].setZero();
            }
        }

        size_t stride = _numBins + 1;
        float c1[ Math::sqr( stride ) ];
        float c2[ ( _numBins + 1 ) * ( _numBins + 1 ) ];
        for( size_t y = 0; y < _numBins+1; y++ ) {
            for( size_t x = 0; x < _numBins+1; x++ ) {
                float jval = _jointHistogram[ y * ( _numBins + 1 ) + x ] + 1e-10f;
                float tval = _templateHistogram[ y ] + 1e-10f;
                c1[ y ][ x ] = Math::log( jval / tval );
                c2[ y ][ x ] = 1.0f / jval;
            }
        }


        _hessian.setZero();

        float sumJ, sumH;

        // go over each point
        for( size_t i = 0; i < _pixelValues.size(); i++ ){
            const float & pixValue = _pixelValues[ i ];
            const JacType & j = _jacobians[ i ];
            const HessianType& h = _hessian[ i ];
            const Vector4f& dw  = _splineDerivativeWeights[ i ];
            const Vector4f& dw2 = _splineSecondDerivativeWeights[ i ];
            const Vector4f& w = _splineWeights[ i ];

            float t = pixValue * (float)( _numBins - 3 ) + 1.0f;
            int tidx = ( int )t;

            sumJ = 0.0f;
            sumH = 0.0f;
            for( int m = -1; m <= 2; m++ ) {
                float& spl = w[ m + 1 ];
                for( int o = -1; o <= 2; o++ ) {
                    sumJ +=  spl *  dw[ o + 1 ] * c1[ tidx + o ][ tidx + m ];
                    sumH +=  spl * dw2[ o + 1 ] * c1[ tidx + o ][ tidx + m ];
                    allJac[ tidx + o ][ tidx + m ] += spl * dw[ o + 1 ] * j / norm;
                }
            }
            _hessian += h * sumJ + sumH * _jTempOuter[ y * w + x ];

            pi += stride;
        }

        _miHessian /= norm;
        _warped.unmap( ptr );

        for( size_t y = 0; y < _numBins+1; y++ ) {
            for( size_t x = 0; x < _numBins+1; x++ ) {
                _miHessian += c2[ y ][ x ] * allJac[ y ][ x ] * allJac[ y ][ x ].transpose();
            }
        }
        _miHessian -= _hOfflineTemp;
        _hessApprox = _miHessian.inverse();*/
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
