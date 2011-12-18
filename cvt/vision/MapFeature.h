#ifndef CVT_MAP_FEATURE_H
#define CVT_MAP_FEATURE_H

#include <Eigen/Core>
#include <set>

namespace cvt
{
	class MapFeature
	{
		public:

			typedef std::set<size_t>::const_iterator ConstPointTrackIterator;

			MapFeature( const Eigen::Vector4d & p, const Eigen::Matrix4d & covariance );

			Eigen::Vector4d&		estimate()		   { return _point; }
			const Eigen::Vector4d&	estimate()   const { return _point; }
			Eigen::Matrix4d&		covariance()	   { return _covariance; }
			const Eigen::Matrix4d&	covariance() const { return _covariance; }

			ConstPointTrackIterator pointTrackBegin() const	{ return _pointTrack.begin(); };
			ConstPointTrackIterator pointTrackEnd() const	{ return _pointTrack.end();   };

			void addPointTrack( size_t camId ) { _pointTrack.insert( camId ); }


		private:
			Eigen::Vector4d		_point;
			Eigen::Matrix4d		_covariance;

			// camera ids which have measurements of this point
			std::set<size_t>	_pointTrack;

	};
			
	inline MapFeature::MapFeature( const Eigen::Vector4d & p, 
								   const Eigen::Matrix4d & covariance ):
		_point( p ),
		_covariance( covariance )
	{
	}
}

#endif
