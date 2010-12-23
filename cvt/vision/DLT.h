#ifndef CVT_DLT_H
#define CVT_DLT_H

#include <vector>
#include <Eigen/Core>

namespace cvt
{
	class DLT
	{
		public:
			/* standard dlt, without point normalization */
			static void dlt( std::vector<Eigen::Vector2d> & reference, 
					   		 std::vector<Eigen::Vector2d> & transformed,
							 Eigen::Matrix3d & H );
							
			static void dlt( const Eigen::Matrix<double, 3, Eigen::Dynamic> & reference,
			 				 const Eigen::Matrix<double, 3, Eigen::Dynamic> & transformed,
							 Eigen::Matrix3d & H);

			/* normalized dlt versions: you should use this!*/
			static void ndlt( std::vector<Eigen::Vector2d> & reference, 
							  std::vector<Eigen::Vector2d> & transformed,
							  Eigen::Matrix3d & H );

			static void ndlt( const Eigen::Matrix<double, 3, Eigen::Dynamic> & reference,
			 				  const Eigen::Matrix<double, 3, Eigen::Dynamic> & transformed,
							  Eigen::Matrix3d & H );							
							
		private:
			static void normalize( const std::vector<Eigen::Vector2d> & pts,
			 					   std::vector<Eigen::Vector2d> & normalizedPts,
								   Eigen::Matrix3d & similarity );
								
			static void normalize( const Eigen::Matrix<double, 3, Eigen::Dynamic> & pts,
								   Eigen::Matrix<double, 3, Eigen::Dynamic> & normalizedPts,
			 					   Eigen::Matrix3d & similarity );
			DLT();
	};
	
}

#endif
