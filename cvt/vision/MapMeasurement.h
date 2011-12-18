#ifndef CVT_MAP_MEASUREMENT_H
#define CVT_MAP_MEASUREMENT_H

namespace cvt
{
	class MapMeasurement
	{
		public:
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
