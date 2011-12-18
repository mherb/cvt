#ifndef CVT_KEYFRAME_H
#define CVT_KEYFRAME_H

#include <Eigen/Core>
#include <map>

#include <cvt/vision/MapMeasurement.h>

namespace cvt
{
	class Keyframe
	{
		public:
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

			typedef std::map<size_t, MapMeasurement>::const_iterator MeasurementIterator;

			Keyframe( const Eigen::Matrix4d& pose );

			void					addFeature( const MapMeasurement & f, size_t id );

			size_t					numMeasurements()				const { return _featMeas.size(); }
			const MapMeasurement&	measurementForId( size_t id  )	const { return _featMeas.find( id ).second; }

			const Eigen::Matrix4d&	pose()						const { return _pose; }	
			void					setPose( const Eigen::Matrix4d & pose ) { _pose = pose; }

			// distance measure of this keyframe to a given transform
			// double				distance( const Eigen::Matrix4d & transform ) const;
			MeasurementIterator		measurementsBegin() const { return _featMeas.begin(); }
			MeasurementIterator		measurementsEnd()   const { return _featMeas.end(); }

		private:
			Eigen::Matrix4d						_pose;

			// 2d meas of 3d feat with id
			std::map<size_t, MapMeasurement>	_featMeas;
	};

	inline void Keyframe::addFeature( const MapMeasurement & f, size_t id )
	{
		_features.insert( std::pair<size_t, MapMeasurement>( id, f ) );
	}
}

#endif
