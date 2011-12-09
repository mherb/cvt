#ifndef CVT_MAP_FEATURE_H
#define CVT_MAP_FEATURE_H

#include <Eigen/Core>

namespace cvt
{
	class MapFeature
	{
		public:
			MapFeature( const Eigen::Vector4d & p, const Eigen::Matrix4d & covariance );

			Eigen::Vector4d&		estimate()		   { return _point; }
			const Eigen::Vector4d&	estimate()   const { return _point; }
			Eigen::Matrix4d&		covariance()	   { return _covariance; }
			const Eigen::Matrix4d&	covariance() const { return _covariance; }

		private:
			Eigen::Vector4d	_point;
			Eigen::Matrix4d	_covariance;

	};
}


#endif
