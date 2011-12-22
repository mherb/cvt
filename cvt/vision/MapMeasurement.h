#ifndef CVT_MAP_MEASUREMENT_H
#define CVT_MAP_MEASUREMENT_H

#include <Eigen/Core>

namespace cvt
{
	class MapMeasurement
	{
		public:
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW

			MapMeasurement() :
				point( 0, 0 ),
				information( Eigen::Matrix2d::Identity() )
			{
			}

			Eigen::Vector2d	point;

			// information is inverse covariance
			Eigen::Matrix2d	information;

	};
}

#endif
