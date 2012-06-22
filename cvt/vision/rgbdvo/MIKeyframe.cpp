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
        _logFactors         = new float[ Math::sqr( _numBins + 1 ) ];

        computeJacobians( depth, K, params );
    }

    MIKeyframe::~MIKeyframe()
    {        
        if( _jointHistogram )
            delete[] _jointHistogram;

        if( _templateHistogram )
            delete[] _templateHistogram;

        if( _logFactors )
            delete[] _logFactors;
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

        SIMD* simd = SIMD::instance();
        simd->SetValue1f( _templateHistogram, 0.0f, _numBins + 1 );
        clearJointHistogram( simd );

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

                    _templateBins.push_back( ( int )pixVal );

                    float binIdx = _templateBins.back() - 1.0f - pixVal;
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
        simd->MulValue1f( _templateHistogram, _templateHistogram, norm, _numBins + 1 );
        simd->MulValue1f( _jointHistogram, _jointHistogram, norm, Math::sqr( _numBins + 1 ) );

        // compute the template hessian from the stored quantities
        evaluateApproximateHessian();
    }

    void MIKeyframe::addToHistograms( int bin, const Vector4f& weights )
    {        
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

    void MIKeyframe::evaluateApproximateHessian()
    {
        const size_t binsSqr = Math::sqr( _numBins + 1 );
        const size_t binsPerRow = _numBins + 1;

        // we need this, as we have to compute dpii/ddelta^T*dpii/ddelta
        JacType* jacobianHistogram = new JacType[ binsSqr ];
        for( size_t b = 0; b < binsSqr; b++ ) {
            jacobianHistogram[ b ].setZero();
        }

        // precompute the factor, for each r, t combo log( p_I*I* / p_I* )
        updateLogFactors();

        _hessian.setZero();
        float sumJ, sumH;

        // go over each point
        for( size_t i = 0; i < _pixelValues.size(); i++ ){
            const float & pixValue = _pixelValues[ i ];
            const JacType & j    = _jacobians[ i ];
            const HessianType& h = _hessians[ i ];
            const Vector4f& dw   = _splineDerivativeWeights[ i ];
            const Vector4f& dw2  = _splineSecondDerivativeWeights[ i ];
            const Vector4f& w    = _splineWeights[ i ];

            // t and r are the same (initially) as the images are the same
            float tmp = pixValue * ( float )( _numBins - 3 ) + 1.0f;
            int tidx = ( int )tmp;

            sumJ = 0.0f;
            sumH = 0.0f;

            float* c1 = _logFactors + binsPerRow * ( tidx - 1 );
            JacType* jacRow = jacobianHistogram + binsPerRow * ( tidx - 1 );
            for( int r = -1; r < 3; r++ ) {
                float spl = w[ r + 1 ]; // the precomputed spline weight
                for( int t = -1; t < 3; t++ ) {
                    sumJ += spl * dw2[ t + 1 ] * c1[ tidx + t ];
                    spl *= dw[ t + 1 ];
                    sumH += spl * c1[ tidx + t ];
                    jacRow[ tidx + t ] += spl * j;
                }
                c1 += binsPerRow;
                jacRow += binsPerRow;
            }

            //std::cout << "HCoeff: " << sumH << " JCoeff: " << sumJ << std::endl;
            _hessian += ( sumJ * j.transpose() * j - sumH * h );
        }

        std::cout << "Before JTJ: " << _hessian << std::endl;
        _hessian /= ( float )_pixelValues.size();

        JacType* currJ = jacobianHistogram;

        float* joint = _jointHistogram;
        HessianType hsum = HessianType::Zero();
        for( size_t r = 0; r < binsPerRow; r++ ) {
            for( size_t t = 0; t < binsPerRow; t++ ) {
                hsum +=  ( 1.0f / *joint - 1.0f / _templateHistogram[ t ] ) * currJ->transpose() * *currJ;
                joint++;
                currJ++;
            }            
        }
        hsum /= Math::sqr( ( float )_pixelValues.size() );
        hsum += _hessian;

        std::cout << hsum << std::endl;
        _hessian = hsum.inverse();
        std::cout << _hessian << std::endl;

        delete[] jacobianHistogram;
    }

    VOResult MIKeyframe::computeRelativePose( PoseRepresentation& predicted,
                                              const Image& gray,
                                              const Matrix3f& intrinsics,
                                              const VOParams& params )
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
        JacType deltaSum;

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

            // remember which points are valid
            std::vector<size_t> validPointIndices;
            validPointIndices.reserve( warpedPts.size() );
            std::vector<Vector4f> imgSplineVals;
            imgSplineVals.reserve( warpedPts.size() );
            std::vector<int> imgBins;
            imgBins.reserve( warpedPts.size() );

            clearJointHistogram( simd );

            // update the joint histogram
            for( size_t i = 0; i < warpedPts.size(); i++ ){
                const Vector2f& pw = warpedPts[ i ];
                if( pw.x > 0.0f && pw.x < ( gray.width()  - 1 ) &&
                    pw.y > 0.0f && pw.y < ( gray.height() - 1 ) ){
                    float v = interpolatePixelValue( pw, grayMap.ptr(), floatStride );

                    // update the joint histogram using v and pixVals[ i ]
                    v = v * ( _numBins - 3 ) + 1.0f;
                    int r = (int)v;
                    v = (int)v - v; // fraction part

                    Vector4f rvals;
                    rvals[ 0 ] = BSplinef::eval( v - 1.0f );
                    rvals[ 1 ] = BSplinef::eval( v );
                    rvals[ 2 ] = BSplinef::eval( v + 1.0f );
                    rvals[ 3 ] = BSplinef::eval( v + 2.0f );

                    const Vector4f & tvals = _splineWeights[ i ];

                    addToJointHistogram( r, _templateBins[ i ], rvals, tvals );

                    validPointIndices.push_back( i );
                    imgSplineVals.push_back( rvals );
                    imgBins.push_back( r );
                }
            }

            // normalize the joint histogram:
            float normalizer = 1.0f / validPointIndices.size();
            simd->MulValue1f( _jointHistogram, _jointHistogram, normalizer, Math::sqr( _numBins + 1 ) );

            // evaluate the log factors for faster computation
            updateLogFactors();

            // evaluate the jacobian:
            const std::vector<size_t>::const_iterator idxEnd = validPointIndices.end();
            int i = 0;
            deltaSum.setZero();
            for( std::vector<size_t>::const_iterator idx = validPointIndices.begin();
                 idx != idxEnd;
                 idx++, i++ ){
                const JacType& j            = _jacobians[ *idx ];
                const Vector4f& tDerivVals  = _splineDerivativeWeights[ *idx ];
                int   t                     = _templateBins[ *idx ];
                int   r                     = imgBins[ i ];
                const Vector4f& rVals       = imgSplineVals[ i ];

                float sum = calcSummedFactors( r, t, rVals, tDerivVals );

                deltaSum += sum * j;
            }
            deltaSum *= -normalizer;
            std::cout << "JSum: " << deltaSum << std::endl;

            // compute the delta step
            SE3<float>::ParameterVectorType deltaP = -_hessian * deltaSum.transpose();
            std::cout << "Delta:\n" << deltaP << std::endl;

            predicted.pose.applyInverse( -deltaP );

            result.iterations++;
            result.numPixels = validPointIndices.size();
        }
        return result;
    }

    void MIKeyframe::clearJointHistogram( const SIMD* simd )
    {
        simd->SetValue1f( _jointHistogram, 0.0f, Math::sqr( _numBins + 1 ) );
    }

    void MIKeyframe::updateLogFactors()
    {
        float* joint = _jointHistogram;
        float* c1 = _logFactors;
        static const size_t binsPerRow = _numBins + 1;
        for( size_t r = 0; r < binsPerRow; r++ ) {
            for( size_t t = 0; t < binsPerRow; t++ ) {
                *c1 = Math::log( LogOffset + *joint / ( _templateHistogram[ t ] + LogOffset ) );
                c1++;
                joint++;
            }
        }
    }

    void MIKeyframe::addToJointHistogram( int rBin, int tBin, const Vector4f& rvals, const Vector4f& tvals )
    {
        for( size_t r = 0; r < 4; r++ ){
            float* jrow = _jointHistogram + ( rBin - 1 + r ) * ( _numBins + 1 );
            jrow[ tBin - 1 ] += rvals[ r ] * tvals[ 0 ];
            jrow[ tBin     ] += rvals[ r ] * tvals[ 1 ];
            jrow[ tBin + 1 ] += rvals[ r ] * tvals[ 2 ];
            jrow[ tBin + 2 ] += rvals[ r ] * tvals[ 3 ];
        }
    }

    float MIKeyframe::calcSummedFactors( int r, int t, const Vector4f& rVals, const Vector4f& tVals )
    {
        float sum = 0.0f;
        static const size_t stride = _numBins + 1;

        const float* logRow = _logFactors + ( r - 1 ) * stride;
        for( size_t y = 0; y < 4; y++ ){
            float tsum = 0.0f;
            tsum += logRow[ t - 1 ] * tVals[ 0 ];
            tsum += logRow[ t     ] * tVals[ 1 ];
            tsum += logRow[ t + 1 ] * tVals[ 2 ];
            tsum += logRow[ t + 2 ] * tVals[ 3 ];
            sum += rVals[ y ] * tsum;
            logRow += stride;
        }
        return sum;
    }
}
