#include <cvt/vision/EPnP.h>

namespace cvt
{
	typedef EPnP<float>  EPnPf;
	typedef EPnP<double> EPnPd;

	template EPnP<float>::EPnP( const PointSet<3, float>& );
	template EPnP<double>::EPnP( const PointSet<3, double>& );

	template <typename T>
	inline EPnP<T>::EPnP( const PointSet<3, T> & pointSet ) : _points3D( pointSet )
	{
		// controlpoints
		computeControlPoints( pointSet );
		computeBarycentricCoords( pointSet );
	}

	template void EPnP<float>::solve( Matrix4<float>&, const PointSet<2, float>&, const Matrix3<float>& ) const;
	template void EPnP<double>::solve( Matrix4<double>&, const PointSet<2, double>&, const Matrix3<double>& ) const;
	template <typename T>
	inline void EPnP<T>::solve( Matrix4<T> & transform, const PointSet<2, T> & pointSet, const Matrix3<T> & K ) const
	{
		Eigen::Matrix<T, 12, 12> A;

		// build the matrix (M^T*M in the paper)
		buildSystem( A, pointSet, K );

		// compute the svd: we know that it's symmetric and square, so no preconditioning!
		Eigen::JacobiSVD<Eigen::Matrix<T, 12, 12>, Eigen::NoQRPreconditioner> svd( A, Eigen::ComputeFullU | Eigen::ComputeFullV );
		const Eigen::Matrix<T, 12, 12> & V = svd.matrixV();
		const Eigen::Matrix<T, 12, 1> & v0 = V.col( 11 );
		const Eigen::Matrix<T, 12, 1> & v1 = V.col( 10 );
		const Eigen::Matrix<T, 12, 1> & v2 = V.col(  9 );

		// distances of the control points
		Eigen::Matrix<T, 6, 1> controlPointDistances, cpDistSqr;
		computeControlPointsDelta( controlPointDistances );

		Matrix4<T> Tout[ 4 ];
		Matrix4<T> K44;
		K44.setZero();

		const Eigen::Matrix<T, 12, 1> & svals = svd.singularValues();
		std::cout << "Singular Values: " << svals[ 11 ] << ", " << svals[ 10 ] << ", " << svals[ 9 ] << std::endl;

		for( size_t i = 0; i < 3; i++ )
			for( size_t k = 0; k < 3; k++ )
			K44[ i ][ k ] = K[ i ][ k ];
		K44[ 3 ][ 3 ] = 1;

		T	err0, err1, err2;
		Eigen::Matrix<T, 12, 1> combinedV;

		// case N = 1: 
		T beta1 = computeBeta1( v0, controlPointDistances );
		combinedV = beta1 * v0;
		computePose( Tout[ 0 ], combinedV, _controlPoints );
		err0 = reprojectionError( Tout[ 0 ], K44, _points3D, pointSet );		
		std::cout << "T0" << Tout[ 0 ] << std::endl;
		std::cout << "Reprojection Error N=1: " << err0 << std::endl;

		cpDistSqr = controlPointDistances.array() * controlPointDistances.array();

		// N=2;
		Vector2<T> betas2;
		computeBetas2( betas2, v0, v1, cpDistSqr );		
		combinedV = betas2[ 0 ] * v0 + betas2[ 1 ] * v1;
		
		computePose( Tout[ 1 ], combinedV, _controlPoints );
		err1 = reprojectionError( Tout[ 1 ], K44, _points3D, pointSet );
		std::cout << "Reprojection Error N=2: " << err1 << std::endl;
		std::cout << "T1" << Tout[ 1 ] << std::endl;

		// N=3;
		Vector3<T> betas3;
		computeBetas3( betas3, v0, v1, v2, cpDistSqr );
		combinedV = betas3[ 0 ] * v0 + betas3[ 1 ] * v1 + betas3[ 2 ] * v2;
		computePose( Tout[ 2 ], combinedV, _controlPoints );

		err2 = reprojectionError( Tout[ 2 ], K44, _points3D, pointSet );
		std::cout << "Reprojection Error N=3: " << err2 << std::endl;

		for( size_t i = 0; i < 3; i++ ){
			std::cout << "T" << i << ": \n" << Tout[ i ] << std::endl;
		}

		size_t i = 0; 
		if( err1 < err0 ){
			if( err2 < err1 )
				i = 2;
			else 
				i = 1;
		}
		
		transform = Tout[ i ];
	}

	template void EPnP<float>::computeControlPoints( const PointSet<3, float>& );
	template void EPnP<double>::computeControlPoints( const PointSet<3, double>& );
	template <typename T>
	inline void EPnP<T>::computeControlPoints( const PointSet<3, T> & ptSet )
	{
		// the centroid
		PCA<T> pca( 3 );

		Eigen::Matrix<T, Eigen::Dynamic, 1> p( 3, 1 );

		Vector3<T> mean = ptSet.mean();
		_controlPoints.add( mean );

		for( size_t i = 0; i < ptSet.size(); i++ ){
			p[ 0 ] = ptSet[ i ].x;
			p[ 1 ] = ptSet[ i ].y;
			p[ 2 ] = ptSet[ i ].z;
			pca.addSample( p );
		}

		pca.mean( p );
		Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> comp( 3, 3 );
		Eigen::Matrix<T, Eigen::Dynamic, 1> sVals( 3 );
		pca.principleComponents( comp, sVals );

		_controlPoints.add( Vector3<T>( ( T )0, ( T )0, ( T )0 ) );
		_controlPoints.add( Vector3<T>( ( T )0, ( T )0, ( T )0 ) );
		_controlPoints.add( Vector3<T>( ( T )0, ( T )0, ( T )0 ) );
	
		T scale0 = Math::sqrt( sVals[ 0 ] );
				
		_controlPoints[ 1 ].x = scale0 * comp( 0, 0 ) + p[ 0 ];
		_controlPoints[ 1 ].y = scale0 * comp( 1, 0 ) + p[ 1 ];
		_controlPoints[ 1 ].z = scale0 * comp( 2, 0 ) + p[ 2 ];
		
		T scale1 = Math::sqrt( sVals[ 1 ] );
		_controlPoints[ 2 ].x = scale1 * comp( 0, 1 ) + p[ 0 ];
		_controlPoints[ 2 ].y = scale1 * comp( 1, 1 ) + p[ 1 ];
		_controlPoints[ 2 ].z = scale1 * comp( 2, 1 ) + p[ 2 ];

		T scale2 = Math::sqrt( sVals[ 2 ] );
		_controlPoints[ 3 ].x = scale2 * comp( 0, 2 ) + p[ 0 ];
		_controlPoints[ 3 ].y = scale2 * comp( 1, 2 ) + p[ 1 ];
		_controlPoints[ 3 ].z = scale2 * comp( 2, 2 ) + p[ 2 ];

		_pcInv[ 0 ][ 0 ] = comp( 0, 0 ) / scale0; _pcInv[ 0 ][ 1 ] = comp( 1, 0 ) / scale0;	_pcInv[ 0 ][ 2 ] = comp( 2, 0 ) / scale0;
		_pcInv[ 1 ][ 0 ] = comp( 0, 1 ) / scale1; _pcInv[ 1 ][ 1 ] = comp( 1, 1 ) / scale1;	_pcInv[ 1 ][ 2 ] = comp( 2, 1 ) / scale1;
		_pcInv[ 2 ][ 0 ] = comp( 0, 2 ) / scale2; _pcInv[ 2 ][ 1 ] = comp( 1, 2 ) / scale2;	_pcInv[ 2 ][ 2 ] = comp( 2, 2 ) / scale2;
	}
	
	template <typename T>
	inline void EPnP<T>::computeBarycentricCoords( const PointSet<3, T> & ptSet )
	{
		_barycentricCoords.clear();
		_barycentricCoords.reserve( ptSet.size() );
		Vector3<T> tmp;
		T alpha0;
		for( size_t i = 0; i < ptSet.size(); i++ ){
			tmp = _pcInv * ( ptSet[ i ] - _controlPoints[ 0 ] );
			alpha0 = ( T )1 - tmp[ 0 ] - tmp[ 1 ] - tmp[ 2 ];
			_barycentricCoords.push_back( Vector4<T>( alpha0, tmp.x, tmp.y, tmp.z ) );
		}
	}

	template<typename T>
	inline void EPnP<T>::buildSystem( Eigen::Matrix<T, 12, 12> & A, const PointSet<2, T> & points2D, const Matrix3<T> & K ) const
	{
		// build the matrix:
		A.setZero();

		Eigen::Matrix<T, 12, 1> l0, l1;		
		for( size_t i = 0; i < points2D.size(); i++ ){
			for( size_t k = 0; k < 4; k++ ){
				l0[ k * 3 ]		= _barycentricCoords[ i ][ k ] * K[ 0 ][ 0 ];
				l0[ k * 3 + 1 ] = _barycentricCoords[ i ][ k ] * K[ 0 ][ 1 ];
				l0[ k * 3 + 2 ] = _barycentricCoords[ i ][ k ] * ( K[ 0 ][ 2 ] - points2D[ i ].x );
				l1[ k * 3 ]		= ( T )0; 
				l1[ k * 3 + 1 ] = _barycentricCoords[ i ][ k ] * K[ 1 ][ 1 ];
				l1[ k * 3 + 2 ] = _barycentricCoords[ i ][ k ] * ( K[ 1 ][ 2 ] - points2D[ i ].y );
			}

			A += l0 * l0.transpose();
			A += l1 * l1.transpose();
		}

	}


	template <typename T>
	inline T EPnP<T>::computeBeta1( const Eigen::Matrix<T, 12, 1> & v, const Eigen::Matrix<T, 6, 1> & deltaC ) const
	{
		T sum = 0;
		T ssum = 0;

		T tmp;

		tmp = Math::sqr( v[ 0 ] - v[ 3 ] ) + Math::sqr( v[ 1 ] - v[ 4 ] ) + Math::sqr( v[ 2 ] - v[ 5 ] );
		ssum += tmp;
		sum += Math::sqrt( tmp ) * deltaC[ 0 ];
		
		tmp = Math::sqr( v[ 0 ] - v[ 6 ] ) + Math::sqr( v[ 1 ] - v[ 7 ] ) + Math::sqr( v[ 2 ] - v[ 8 ] );
		ssum += tmp;
		sum += Math::sqrt( tmp ) * deltaC[ 1 ];

		tmp = Math::sqr( v[ 0 ] - v[ 9 ] ) + Math::sqr( v[ 1 ] - v[ 10 ] ) + Math::sqr( v[ 2 ] - v[ 11 ] );
		ssum += tmp;
		sum += Math::sqrt( tmp ) * deltaC[ 2 ];

		tmp = Math::sqr( v[ 3 ] - v[ 6 ] ) + Math::sqr( v[ 4 ] - v[ 7 ] ) + Math::sqr( v[ 5 ] - v[ 8 ] );
		ssum += tmp;
		sum += Math::sqrt( tmp ) * deltaC[ 3 ];

		tmp = Math::sqr( v[ 3 ] - v[ 9 ] ) + Math::sqr( v[ 4 ] - v[ 10 ] ) + Math::sqr( v[ 5 ] - v[ 11 ] );
		ssum += tmp;
		sum += Math::sqrt( tmp ) * deltaC[ 4 ];

		tmp = Math::sqr( v[ 6 ] - v[ 9 ] ) + Math::sqr( v[ 7 ] - v[ 10 ] ) + Math::sqr( v[ 8 ] - v[ 11 ] );
		ssum += tmp;
		sum += Math::sqrt( tmp ) * deltaC[ 5 ];

		return sum / ssum; 
	}

	template <typename T>
	inline void EPnP<T>::computeBetas2( Vector2<T> & betas, 
										const Eigen::Matrix<T, 12, 1> & v0,
									    const Eigen::Matrix<T, 12, 1> & v1,
										const Eigen::Matrix<T, 6, 1> & cpDistanceSqr ) const
	{
		Eigen::Matrix<T, 6, 3> L;
		Eigen::Matrix<T, 3, 1> delta0, delta1;

		// c0 - c1
		delta0 = v0.block( 0, 0, 3, 1 ) - v0.block( 3, 0, 3, 1 ); 
		delta1 = v1.block( 0, 0, 3, 1 ) - v1.block( 3, 0, 3, 1 );
		L( 0, 0 ) = delta0.dot( delta0 ); 
		L( 0, 1 ) = 2 * delta0.dot( delta1 ); 
		L( 0, 2 ) = delta1.dot( delta1 );
		
		// c0 - c2
		delta0 = v0.block( 0, 0, 3, 1 ) - v0.block( 6, 0, 3, 1 ); 
		delta1 = v1.block( 0, 0, 3, 1 ) - v1.block( 6, 0, 3, 1 );
		L( 1, 0 ) = delta0.dot( delta0 ); 
		L( 1, 1 ) = 2 * delta0.dot( delta1 ); 
		L( 1, 2 ) = delta1.dot( delta1 );

		// c0 - c3
		delta0 = v0.block( 0, 0, 3, 1 ) - v0.block( 9, 0, 3, 1 ); 
		delta1 = v1.block( 0, 0, 3, 1 ) - v1.block( 9, 0, 3, 1 );
		L( 2, 0 ) = delta0.dot( delta0 ); 
		L( 2, 1 ) = 2 * delta0.dot( delta1 ); 
		L( 2, 2 ) = delta1.dot( delta1 );

		// c1 - c2
		delta0 = v0.block( 3, 0, 3, 1 ) - v0.block( 6, 0, 3, 1 ); 
		delta1 = v1.block( 3, 0, 3, 1 ) - v1.block( 6, 0, 3, 1 );
		L( 3, 0 ) = delta0.dot( delta0 ); 
		L( 3, 1 ) = 2 * delta0.dot( delta1 ); 
		L( 3, 2 ) = delta1.dot( delta1 );
		
		// c1 - c3
		delta0 = v0.block( 3, 0, 3, 1 ) - v0.block( 9, 0, 3, 1 ); 
		delta1 = v1.block( 3, 0, 3, 1 ) - v1.block( 9, 0, 3, 1 );
		L( 4, 0 ) = delta0.dot( delta0 ); 
		L( 4, 1 ) = 2 * delta0.dot( delta1 ); 
		L( 4, 2 ) = delta1.dot( delta1 );

		// c2 - c3
		delta0 = v0.block( 6, 0, 3, 1 ) - v0.block( 9, 0, 3, 1 ); 
		delta1 = v1.block( 6, 0, 3, 1 ) - v1.block( 9, 0, 3, 1 );
		L( 5, 0 ) = delta0.dot( delta0 ); 
		L( 5, 1 ) = 2 * delta0.dot( delta1 ); 
		L( 5, 2 ) = delta1.dot( delta1 );

		Eigen::Matrix<T, 3, 1> x = ( L.transpose() * L ).inverse() * L.transpose() * cpDistanceSqr;

		T b0 = Math::sqrt( x[ 0 ] );
		T b1 = Math::sqrt( x[ 2 ] );

		Eigen::Matrix<T, 12, 1> vtest;
		if( x[ 1 ] > 0 ){
			int posCount0, posCount1;
			// check ++
			vtest = b0 * v0 + b1 * v1;
			posCount0 = ( vtest[  2 ] > 0 ? 1 : 0 ) +
						( vtest[  5 ] > 0 ? 1 : 0 ) +
						( vtest[  8 ] > 0 ? 1 : 0 ) +
						( vtest[ 11 ] > 0 ? 1 : 0 );

			vtest = -b0 * v0 - b1 * v1;
			posCount1 = ( vtest[  2 ] > 0 ? 1 : 0 ) +
						( vtest[  5 ] > 0 ? 1 : 0 ) +
						( vtest[  8 ] > 0 ? 1 : 0 ) +
						( vtest[ 11 ] > 0 ? 1 : 0 );
			if( posCount0 > posCount1 ){
				betas[ 0 ] = b0;
				betas[ 1 ] = b1;
			} else {
				betas[ 0 ] = -b0;
				betas[ 1 ] = -b1;
			}
		} else {
			int posCount0, posCount1;
			// check +-
			vtest = b0 * v0 - b1 * v1;
			posCount0 = ( vtest[  2 ] > 0 ? 1 : 0 ) +
						( vtest[  5 ] > 0 ? 1 : 0 ) +
						( vtest[  8 ] > 0 ? 1 : 0 ) +
						( vtest[ 11 ] > 0 ? 1 : 0 );

			vtest = -b0 * v0 + b1 * v1;
			posCount1 = ( vtest[  2 ] > 0 ? 1 : 0 ) +
						( vtest[  5 ] > 0 ? 1 : 0 ) +
						( vtest[  8 ] > 0 ? 1 : 0 ) +
						( vtest[ 11 ] > 0 ? 1 : 0 );
			if( posCount0 > posCount1 ){
				betas[ 0 ] =  b0;
				betas[ 1 ] = -b1;
			} else {
				betas[ 0 ] = -b0;
				betas[ 1 ] =  b1;
			}
		}
	}

	template <typename T>
	inline void EPnP<T>::computeBetas3( Vector3<T> & betas,
									    const Eigen::Matrix<T, 12, 1> & v0,
									    const Eigen::Matrix<T, 12, 1> & v1,
									    const Eigen::Matrix<T, 12, 1> & v2,
										const Eigen::Matrix<T,  6, 1> & cpDistanceSqr ) const
	{
		Eigen::Matrix<T, 6, 6> L;
		Eigen::Matrix<T, 3, 1> d0, d1, d2;
		Eigen::Matrix<T, 6, 1> b;

		// c0 - c1
		d0 = v0.block( 0, 0, 3, 1 ) - v0.block( 3, 0, 3, 1 ); 
		d1 = v1.block( 0, 0, 3, 1 ) - v1.block( 3, 0, 3, 1 );
		d2 = v2.block( 0, 0, 3, 1 ) - v2.block( 3, 0, 3, 1 );
		L( 5, 0 ) = d0.dot( d0 ); 
		L( 5, 1 ) = 2 * d0.dot( d1 );
		L( 5, 2 ) = 2 * d0.dot( d2 );
		L( 5, 3 ) = 2 * d1.dot( d2 );
		L( 5, 4 ) = d1.dot( d1 );
		L( 5, 5 ) = d2.dot( d2 );
		
		// c0 - c2
		d0 = v0.block( 0, 0, 3, 1 ) - v0.block( 6, 0, 3, 1 ); 
		d1 = v1.block( 0, 0, 3, 1 ) - v1.block( 6, 0, 3, 1 );
		d2 = v2.block( 0, 0, 3, 1 ) - v2.block( 6, 0, 3, 1 );
		L( 5, 0 ) = d0.dot( d0 ); 
		L( 5, 1 ) = 2 * d0.dot( d1 );
		L( 5, 2 ) = 2 * d0.dot( d2 );
		L( 5, 3 ) = 2 * d1.dot( d2 );
		L( 5, 4 ) = d1.dot( d1 );
		L( 5, 5 ) = d2.dot( d2 );

		// c0 - c3
		d0 = v0.block( 0, 0, 3, 1 ) - v0.block( 9, 0, 3, 1 ); 
		d1 = v1.block( 0, 0, 3, 1 ) - v1.block( 9, 0, 3, 1 );
		d2 = v2.block( 0, 0, 3, 1 ) - v2.block( 9, 0, 3, 1 );
		L( 5, 0 ) = d0.dot( d0 ); 
		L( 5, 1 ) = 2 * d0.dot( d1 );
		L( 5, 2 ) = 2 * d0.dot( d2 );
		L( 5, 3 ) = 2 * d1.dot( d2 );
		L( 5, 4 ) = d1.dot( d1 );
		L( 5, 5 ) = d2.dot( d2 );

		// c1 - c2
		d0 = v0.block( 3, 0, 3, 1 ) - v0.block( 6, 0, 3, 1 ); 
		d1 = v1.block( 3, 0, 3, 1 ) - v1.block( 6, 0, 3, 1 );
		d2 = v2.block( 3, 0, 3, 1 ) - v2.block( 6, 0, 3, 1 );
		L( 5, 0 ) = d0.dot( d0 ); 
		L( 5, 1 ) = 2 * d0.dot( d1 );
		L( 5, 2 ) = 2 * d0.dot( d2 );
		L( 5, 3 ) = 2 * d1.dot( d2 );
		L( 5, 4 ) = d1.dot( d1 );
		L( 5, 5 ) = d2.dot( d2 );
		
		// c1 - c3
		d0 = v0.block( 3, 0, 3, 1 ) - v0.block( 9, 0, 3, 1 ); 
		d1 = v1.block( 3, 0, 3, 1 ) - v1.block( 9, 0, 3, 1 );
		d2 = v2.block( 3, 0, 3, 1 ) - v2.block( 9, 0, 3, 1 );
		L( 5, 0 ) = d0.dot( d0 ); 
		L( 5, 1 ) = 2 * d0.dot( d1 );
		L( 5, 2 ) = 2 * d0.dot( d2 );
		L( 5, 3 ) = 2 * d1.dot( d2 );
		L( 5, 4 ) = d1.dot( d1 );
		L( 5, 5 ) = d2.dot( d2 );

		// c2 - c3
		d0 = v0.block( 6, 0, 3, 1 ) - v0.block( 9, 0, 3, 1 ); 
		d1 = v1.block( 6, 0, 3, 1 ) - v1.block( 9, 0, 3, 1 );
		d2 = v2.block( 6, 0, 3, 1 ) - v2.block( 9, 0, 3, 1 );
		L( 5, 0 ) = d0.dot( d0 ); 
		L( 5, 1 ) = 2 * d0.dot( d1 );
		L( 5, 2 ) = 2 * d0.dot( d2 );
		L( 5, 3 ) = 2 * d1.dot( d2 );
		L( 5, 4 ) = d1.dot( d1 );
		L( 5, 5 ) = d2.dot( d2 );

		b = L.ldlt().solve( cpDistanceSqr );

		T b0 = Math::sqrt( b[ 0 ] );
		T b1 = Math::sqrt( b[ 4 ] );
		T b2 = Math::sqrt( b[ 5 ] );

		Eigen::Matrix<T, 12, 1> vtmp;
		
		// select the correct sign: 
		int posCount0, posCount1;
		// +++
		if( b[ 1 ] > 0 && b[ 2 ] > 0 && b[ 3 ] > 0 ){
			// either all positive or all negative:
			vtmp = b0 * v0 + b1 * v1 + b2 * v2;
			posCount0 = ( vtmp[  2 ] > 0 ? 1 : 0 ) + ( vtmp[  5 ] > 0 ? 1 : 0 ) +
						( vtmp[  8 ] > 0 ? 1 : 0 ) + ( vtmp[ 11 ] > 0 ? 1 : 0 );

			vtmp = -b0 * v0 -b1 * v1 - b2 * v2;
			posCount1 = ( vtmp[  2 ] > 0 ? 1 : 0 ) + ( vtmp[  5 ] > 0 ? 1 : 0 ) +
						( vtmp[  8 ] > 0 ? 1 : 0 ) + ( vtmp[ 11 ] > 0 ? 1 : 0 );

			if( posCount0 > posCount1 ){
				betas[ 0 ] =  b0; betas[ 1 ] =  b1; betas[ 2 ] =  b2;
			} else {
				betas[ 0 ] = -b0; betas[ 1 ] = -b1; betas[ 2 ] = -b2;
			}
			return;
		}

		// +--
		if( b[ 1 ] > 0 && b[ 2 ] < 0 && b[ 3 ] < 0 ){
			// ++-
			vtmp = b0 * v0 + b1 * v1 - b2 * v2;
			posCount0 = ( vtmp[  2 ] > 0 ? 1 : 0 ) + ( vtmp[  5 ] > 0 ? 1 : 0 ) +
						( vtmp[  8 ] > 0 ? 1 : 0 ) + ( vtmp[ 11 ] > 0 ? 1 : 0 );

			// --+
			vtmp = -b0 * v0 -b1 * v1 + b2 * v2;
			posCount1 = ( vtmp[  2 ] > 0 ? 1 : 0 ) + ( vtmp[  5 ] > 0 ? 1 : 0 ) +
						( vtmp[  8 ] > 0 ? 1 : 0 ) + ( vtmp[ 11 ] > 0 ? 1 : 0 );

			if( posCount0 > posCount1 ){
				betas[ 0 ] =  b0; betas[ 1 ] =  b1; betas[ 2 ] = -b2;
			} else {
				betas[ 0 ] = -b0; betas[ 1 ] = -b1; betas[ 2 ] =  b2;
			}
			return;
		}

		// -+-
		if( b[ 1 ] < 0 && b[ 2 ] > 0 && b[ 3 ] < 0 ){
			// +-+
			vtmp = b0 * v0 - b1 * v1 + b2 * v2;
			posCount0 = ( vtmp[  2 ] > 0 ? 1 : 0 ) + ( vtmp[  5 ] > 0 ? 1 : 0 ) +
						( vtmp[  8 ] > 0 ? 1 : 0 ) + ( vtmp[ 11 ] > 0 ? 1 : 0 );

			// -+-
			vtmp = -b0 * v0 + b1 * v1 - b2 * v2;
			posCount1 = ( vtmp[  2 ] > 0 ? 1 : 0 ) + ( vtmp[  5 ] > 0 ? 1 : 0 ) +
						( vtmp[  8 ] > 0 ? 1 : 0 ) + ( vtmp[ 11 ] > 0 ? 1 : 0 );

			if( posCount0 > posCount1 ){
				betas[ 0 ] =  b0; betas[ 1 ] = -b1; betas[ 2 ] =  b2;
			} else {
				betas[ 0 ] = -b0; betas[ 1 ] =  b1; betas[ 2 ] = -b2;
			}
			return;
		}

		
		// --+ for sure!
		// -++
		vtmp = -b0 * v0 + b1 * v1 + b2 * v2;
		posCount0 = ( vtmp[  2 ] > 0 ? 1 : 0 ) + ( vtmp[  5 ] > 0 ? 1 : 0 ) +
					( vtmp[  8 ] > 0 ? 1 : 0 ) + ( vtmp[ 11 ] > 0 ? 1 : 0 );

		// +--
		vtmp = b0 * v0 - b1 * v1 - b2 * v2;
		posCount1 = ( vtmp[  2 ] > 0 ? 1 : 0 ) + ( vtmp[  5 ] > 0 ? 1 : 0 ) +
					( vtmp[  8 ] > 0 ? 1 : 0 ) + ( vtmp[ 11 ] > 0 ? 1 : 0 );

		if( posCount0 > posCount1 ){
			betas[ 0 ] = -b0; betas[ 1 ] =  b1; betas[ 2 ] =  b2;
		} else {
			betas[ 0 ] =  b0; betas[ 1 ] = -b1; betas[ 2 ] = -b2;
		}
	}

	template <typename T>
	inline void EPnP<T>::computeControlPointsDelta( Eigen::Matrix<T, 6, 1> & cpDelta ) const
	{
		cpDelta[ 0 ] = ( _controlPoints[ 0 ] - _controlPoints[ 1 ] ).length();
		cpDelta[ 1 ] = ( _controlPoints[ 0 ] - _controlPoints[ 2 ] ).length();
		cpDelta[ 2 ] = ( _controlPoints[ 0 ] - _controlPoints[ 3 ] ).length();
		cpDelta[ 3 ] = ( _controlPoints[ 1 ] - _controlPoints[ 2 ] ).length();
		cpDelta[ 4 ] = ( _controlPoints[ 1 ] - _controlPoints[ 3 ] ).length();
		cpDelta[ 5 ] = ( _controlPoints[ 2 ] - _controlPoints[ 3 ] ).length();
	}

	template <typename T>
	inline void EPnP<T>::computePose( Matrix4<T> & transform, 
									  const Eigen::Matrix<T, 12, 1> & estimatedCoords, 
									  const PointSet<3, T> & controlPoints ) const
	{
		PointSet<3, T> estimated;
		estimated.add( Vector3<T>( estimatedCoords[ 0 ], estimatedCoords[  1 ], estimatedCoords[  2 ] ) );
		estimated.add( Vector3<T>( estimatedCoords[ 3 ], estimatedCoords[  4 ], estimatedCoords[  5 ] ) );
		estimated.add( Vector3<T>( estimatedCoords[ 6 ], estimatedCoords[  7 ], estimatedCoords[  8 ] ) );
		estimated.add( Vector3<T>( estimatedCoords[ 9 ], estimatedCoords[ 10 ], estimatedCoords[ 11 ] ) );

		transform = controlPoints.alignRigid( estimated );
	}

	template <typename T>
	inline T EPnP<T>::reprojectionError( const Matrix4<T> & transform,
										 const Matrix4<T> & K44,
	  									 const PointSet<3, T> & p3d,
	  									 const PointSet<2, T> & p2d ) const
	{
		T error = ( T )0;
		Matrix4<T> P = K44 * transform;
		Vector3<T> tmp;
		Vector2<T> proj;

		for( size_t i = 0, n = p3d.size(); i < n; i++ ){
			tmp = P * p3d[ i ];
			proj[ 0 ] = tmp[ 0 ] / tmp[ 2 ]; 
			proj[ 1 ] = tmp[ 1 ] / tmp[ 2 ];

			error += ( proj	- p2d[ i ] ).lengthSqr();
		}

		return error;		
	}

}
