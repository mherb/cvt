#ifndef CVT_MAP_FEATURE_H
#define CVT_MAP_FEATURE_H

#include <Eigen/Eigen>

#include <cvt/vision/ORB.h>

namespace cvt
{
	class MapFeature
	{
		public:
			MapFeature(	const Eigen::Vector4d & point );

			void addMeasurement( size_t keyframeId, const ORBFeature & feature );

			const Eigen::Vector4d & worldData() const { return _point3d; }

			size_t id() const { return _id; }

			/* TODO: assumes first occurance is our descriptor! */
			ORBFeature* descriptor() { return &_measurements[ 0 ]; }

		private:
			/* id of the feature?! */
			size_t		_id;

			/* the current estimate stored as homogeneous point */
			Eigen::Vector4d	_point3d;

			/* Ids of the keyframes that have a measurement of this point */
			std::vector<size_t>		_keyframeIds;

			// TODO: split ORBFeature into a position and a Descriptor
			std::vector<ORBFeature>	_measurements;

			static size_t _nextId;

			/* just to be sure */
			MapFeature( const MapFeature& );
			const MapFeature& operator=( const MapFeature& );
	};

	size_t MapFeature::_nextId = 0;

	inline MapFeature::MapFeature( const Eigen::Vector4d & point ) :
		_id( _nextId++ ),
		_point3d( point )
	{
	}

	inline void MapFeature::addMeasurement( size_t keyframeId, const ORBFeature & sp )
	{
		_keyframeIds.push_back( keyframeId );
		_measurements.push_back( sp );
	}
}

#endif
