#ifndef CVT_RANSAC_H
#define CVT_RANSAC_H

#include <set>
#include <cvt/math/Math.h>

namespace cvt
{
	class RANSAC
	{
		public:
			RANSAC( double assumedSigma, double outlierProbability );
			virtual ~RANSAC();
			
			void dlt( const Eigen::Matrix<double, 3, Eigen::Dynamic> & ref,
			 		  const Eigen::Matrix<double, 3, Eigen::Dynamic> & trans,
					  Eigen::Matrix3d & H );
			
		private:
			double _outlierProb;
			double _tSquare;
			size_t _numSamples;
			
			// select n random points from pts
			template<size_t n>
			static void randomSelect( size_t maxIndex, std::set<size_t> & indices );
			
			void estimateWithIndices( const Eigen::Matrix<double, 3, Eigen::Dynamic> & ref,
			 		  				  const Eigen::Matrix<double, 3, Eigen::Dynamic> & trans,
									  std::set<size_t> & indices,
					  				  Eigen::Matrix3d & H );
					
			void inliers( const Eigen::Matrix<double, 3, Eigen::Dynamic> & ref,
						  const Eigen::Matrix<double, 3, Eigen::Dynamic> & trans,
						  Eigen::Matrix3d & H,
						  std::set<size_t> & inlierIndices );
	};
	
	/* TODO: avoid degenerate configurations? */
	template<size_t n>
	inline void RANSAC::randomSelect( size_t maxIndex, std::set<size_t> & indices )
	{
		size_t idx;
		for( size_t i = 0; i < n; i++ ){
			idx = Math::rand( 0, maxIndex );
			
			while( indices.find( idx ) != indices.end() ){
				idx = Math::rand( 0, maxIndex );
			}			
												
			indices.insert( idx );
		}		
	}
}

#endif