#include <cvt/vision/DLT.h>

#include <Eigen/SVD>
#include <Eigen/LU>
#include <Eigen/Array>

namespace cvt
{
	void DLT::ndlt( std::vector<Eigen::Vector2d> & reference, 
				  	std::vector<Eigen::Vector2d> & transformed,
					Eigen::Matrix3d & H )
	{
		Eigen::Matrix3d simRef, simTrans;

		std::vector<Eigen::Vector2d> nRef, nTrans;

		// normalize the point sets
		DLT::normalize( reference, nRef, simRef );
		DLT::normalize( transformed, nTrans, simTrans );

		// run normal dlt
		dlt( nRef, nTrans, H );

		// undo normalization
		H = ( simTrans.inverse() * H * simRef );

		H *= 1.0 / H( 2, 2 );
	}

	void DLT::ndlt( const Eigen::Matrix<double, 3, Eigen::Dynamic> & reference,
	 				const Eigen::Matrix<double, 3, Eigen::Dynamic> & transformed,
					Eigen::Matrix3d & H )
	{
		Eigen::Matrix3d simRef, simTrans;
		Eigen::Matrix<double, 3, Eigen::Dynamic> nRef( 3, reference.cols() );
		Eigen::Matrix<double, 3, Eigen::Dynamic> nTrans( 3, reference.cols() );

		DLT::normalize( reference, nRef, simRef );
		DLT::normalize( transformed, nTrans, simTrans );

		dlt( nRef, nTrans, H );
		H = ( simTrans.inverse() * H * simRef );	
		H *= 1.0 / H( 2, 2 );
	}
	
	void DLT::dlt( const Eigen::Matrix<double, 3, Eigen::Dynamic> & reference,
	 			   const Eigen::Matrix<double, 3, Eigen::Dynamic> & transformed,
				   Eigen::Matrix3d & H )
	{	
		Eigen::MatrixXd A( 2 * reference.cols(), 9 );
		Eigen::VectorXd b = Eigen::VectorXd::Zero( 2 * reference.cols() );
		Eigen::Matrix<double, 9, 1> x;

		for( int32_t i = 0; i < reference.cols(); i++ ){
			A( 2 * i, 0 ) = 0.0; 
			A( 2 * i, 1 ) = 0.0; 
			A( 2 * i, 2 ) = 0.0;

			A( 2 * i, 3 ) = -reference( 0, i ) * transformed( 2, i ); 
			A( 2 * i, 4 ) = -reference( 1, i ) * transformed( 2, i ); 
			A( 2 * i, 5 ) = -reference( 2, i ) * transformed( 2, i );

			A( 2 * i, 6 ) = reference( 0, i ) * transformed( 1, i );
			A( 2 * i, 7 ) = reference( 1, i ) * transformed( 1, i );
			A( 2 * i, 8 ) = reference( 2, i ) * transformed( 1, i );

			A( 2 * i + 1, 0 ) = -A( 2 * i, 3 ); 
			A( 2 * i + 1, 1 ) = -A( 2 * i, 4 ); 
			A( 2 * i + 1, 2 ) = -A( 2 * i, 5 ); 
			A( 2 * i + 1, 3 ) = 0.0; 
			A( 2 * i + 1, 4 ) = 0.0; 
			A( 2 * i + 1, 5 ) = 0.0;
			A( 2 * i + 1, 6 ) = -reference( 0, i ) * transformed( 0, i );
			A( 2 * i + 1, 7 ) = -reference( 1, i ) * transformed( 0, i );
			A( 2 * i + 1, 8 ) = -reference( 2, i ) * transformed( 0, i );		
		}

		Eigen::SVD<Eigen::MatrixXd> svd = A.svd();	
		x = svd.matrixV().col( svd.matrixV().cols() - 1 );

		H( 0, 0 ) = x[ 0 ];	H( 0, 1 ) = x[ 1 ]; H( 0, 2 ) = x[ 2 ];
		H( 1, 0 ) = x[ 3 ];	H( 1, 1 ) = x[ 4 ]; H( 1, 2 ) = x[ 5 ];
		H( 2, 0 ) = x[ 6 ];	H( 2, 1 ) = x[ 7 ]; H( 2, 2 ) = x[ 8 ];

		// normalize last entry to 1
		H *= 1.0 / x[ 8 ];		
	}


	void DLT::dlt( std::vector<Eigen::Vector2d> & reference, 
				   std::vector<Eigen::Vector2d> & transformed,
				   Eigen::Matrix3d & H )
	{
		// we use H33 = 1
		Eigen::MatrixXd A( 2 * reference.size(), 8 );
		Eigen::VectorXd b( 2 * reference.size() );
		Eigen::Matrix<double, 8, 1> x;

		for( size_t i = 0; i < reference.size(); i++ ){
			A( 2 * i, 0 ) = 0.0; 
			A( 2 * i, 1 ) = 0.0; 
			A( 2 * i, 2 ) = 0.0;
			A( 2 * i, 3 ) = -reference[ i ][ 0 ]; 
			A( 2 * i, 4 ) = -reference[ i ][ 1 ]; 

			/* FIXME: this should be -1.0 */

			A( 2 * i, 5 ) = 1.0;
			A( 2 * i, 6 ) = reference[ i ][ 0 ] * transformed[ i ][ 1 ];
			A( 2 * i, 7 ) = reference[ i ][ 1 ] * transformed[ i ][ 1 ];

			A( 2 * i + 1, 0 ) = reference[ i ][ 0 ]; 
			A( 2 * i + 1, 1 ) = reference[ i ][ 1 ]; 
			A( 2 * i + 1, 2 ) = 1.0;
			A( 2 * i + 1, 3 ) = 0.0; 
			A( 2 * i + 1, 4 ) = 0.0; 
			A( 2 * i + 1, 5 ) = 0.0;
			A( 2 * i + 1, 6 ) = -reference[ i ][ 0 ] * transformed[ i ][ 0 ];
			A( 2 * i + 1, 7 ) = -reference[ i ][ 1 ] * transformed[ i ][ 0 ];

			b[ 2 * i ] 		= -transformed[ i ][ 1 ];
			b[ 2 * i + 1 ] 	=  transformed[ i ][ 0 ];
		}

		A.svd().solve( b, &x );

		H( 0, 0 ) = x[ 0 ];	H( 0, 1 ) = x[ 1 ]; H( 0, 2 ) = x[ 2 ];
		H( 1, 0 ) = x[ 3 ];	H( 1, 1 ) = x[ 4 ]; H( 1, 2 ) = x[ 5 ];
		H( 2, 0 ) = x[ 6 ];	H( 2, 1 ) = x[ 7 ]; H( 2, 2 ) = 1.0;
	}

	void DLT::normalize( const Eigen::Matrix<double, 3, Eigen::Dynamic> & pts,
					     Eigen::Matrix<double, 3, Eigen::Dynamic> & normalizedPts,
	 					 Eigen::Matrix3d & similarity )
	{
		similarity = Eigen::Matrix3d::Identity();

		Eigen::Vector3d mean;
		mean = pts.rowwise().sum();
		mean /= pts.cols();

		double scale = 0.0;
		Eigen::Vector2d tmp;
		for( int32_t i = 0; i < pts.cols(); i++ ){

			tmp[ 0 ] = pts( 0, i ) - mean[ 0 ];
			tmp[ 1 ] = pts( 1, i ) - mean[ 1 ];

			scale += tmp.norm();
		}
		scale /= pts.cols();
		scale = -sqrt( 2.0 ) / scale;

		similarity( 0, 0 ) = scale; similarity( 0, 2 ) = mean[ 0 ] * scale;
		similarity( 1, 1 ) = scale; similarity( 1, 2 ) = mean[ 1 ] * scale;

		normalizedPts = similarity * pts;			
	}

	void DLT::normalize( const std::vector<Eigen::Vector2d> & pts,
	 					 std::vector<Eigen::Vector2d> & normalized,
						 Eigen::Matrix3d & similarity )
	{
		Eigen::Vector2d t = Eigen::Vector2d::Zero();
		similarity = Eigen::Matrix3d::Identity();

		for( size_t i = 0; i < pts.size(); i++ ){
			t += pts[ i ]; 
		}

		t *= -1.0 / pts.size();

		double scale = 0.0;
		Eigen::Vector2d p;	
		for( size_t i = 0; i < pts.size(); i++ ){		
			p = pts[ i ] + t;
			scale += p.norm();
			normalized.push_back( p );
		}

		double avgDist = scale / (double)pts.size();
		scale = sqrt( 2.0 ) / avgDist;

		similarity( 0, 0 ) =  scale;
		similarity( 1, 1 ) =  scale;
		t *= scale; 
		similarity.block( 0, 2, 2, 1) = t;

		for( size_t i = 0; i < normalized.size(); i++ ){
			normalized[ i ] *= scale;
		}
	}	
}
