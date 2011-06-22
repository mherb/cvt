#include "RANSAC.h"
#include <cvt/vision/DLT.h>

namespace cvt
{

	RANSAC::RANSAC( double assumedSigma, double outlierProbability ) :
		_outlierProb( outlierProbability )
	{
		/* 5.99 for homographies -> p. 137 MVG */
		_tSquare = 5.99 * Math::sqr( assumedSigma );

		/* N = log( 1 - p ) / log( 1 - ( 1 - epsilon )^s )
		 * p: desired propability, that at least ONE sample is free from outliers
		 * epsilon: prob. of an outlier
		 * s: sample set size
		*/
		_numSamples = log( 0.01 ) / log( 1.0 - pow( 1.0 - outlierProbability, 5 ) );
	}

	RANSAC::~RANSAC()
	{
	}

	void RANSAC::dlt( const Eigen::Matrix<double, 3, Eigen::Dynamic> & ref,
					  const Eigen::Matrix<double, 3, Eigen::Dynamic> & trans,
					  Eigen::Matrix3d & H )
	{
		// best consensus set up to now and current!
		std::set<size_t> consensusSet, currentConsensus;
		Eigen::Matrix3d currentH;

		double expectedInliers = ( 1.0 - _outlierProb ) * ref.cols();

		size_t iter = 0;
		while( true ){
			std::set<size_t> indices;
			indices.clear();
			RANSAC::randomSelect<5>( ref.cols() - 1, indices );

			estimateWithIndices( ref, trans, indices, currentH );

			inliers( ref, trans, currentH, currentConsensus );

			size_t numInliers = currentConsensus.size();

			if( numInliers >= expectedInliers ){
				estimateWithIndices( ref, trans, currentConsensus, H );
				return;
			}

			if( numInliers > consensusSet.size() ){
				consensusSet = currentConsensus;
			}

			if( iter >= _numSamples ){
				if( consensusSet.size() < 5 ){
					// merde ... run standard algo
					DLT::ndlt( ref, trans, H );
				} else {
					estimateWithIndices( ref, trans, consensusSet, H );
				}
				return;
			}

			iter++;
		}
	}

	void RANSAC::estimateWithIndices( const Eigen::Matrix<double, 3, Eigen::Dynamic> & ref,
									  const Eigen::Matrix<double, 3, Eigen::Dynamic> & trans,
									  std::set<size_t> & indices,
									  Eigen::Matrix3d & H )
	{
		std::set<size_t>::iterator it = indices.begin();
		Eigen::Matrix<double, 3, Eigen::Dynamic> selectionRef( 3, indices.size() );
		Eigen::Matrix<double, 3, Eigen::Dynamic> selectionTrans( 3, indices.size() );

		size_t i = 0;
		while( it != indices.end() ){
			selectionRef.col( i ) = ref.col( *it );
			selectionTrans.col( i ) = trans.col( *it );
			it++;
			i++;
		}

		DLT::ndlt( selectionRef, selectionTrans, H );
	}

	void RANSAC::inliers( const Eigen::Matrix<double, 3, Eigen::Dynamic> & ref,
						  const Eigen::Matrix<double, 3, Eigen::Dynamic> & trans,
						  Eigen::Matrix3d & H,
						  std::set<size_t> & inlierIndices )
	{
		Eigen::Matrix<double, 3, Eigen::Dynamic> refT( 3, ref.cols() );
		refT = H * ref;
		refT-=trans;

		for( int32_t i = 0; i < refT.cols(); i++ ){
			if( refT.col( i ).squaredNorm() < _tSquare ){
				inlierIndices.insert( i );
			}
		}
	}
}
