#ifndef CVT_SLAMMAP_H
#define CVT_SLAMMAP_H

#include <cvt/vision/CameraCalibration.h>
#include <cvt/vision/Keyframe.h>
#include <cvt/vision/MapFeature.h>

namespace cvt
{
	class SlamMap
	{
		public:
			SlamMap();
			~SlamMap();

			size_t addKeyframe( const Eigen::Matrix4d& pose );
			size_t addFeature( const MapFeature& world );

			size_t addFeatureToKeyframe( const MapFeature& world,
									     const Eigen::Vector2d& feature,
									     size_t keyframeId );

			void addMeasurement( size_t pointId, 
							     size_t keyframeId,
								 const Eigen::Vector2d& meas );


			size_t findClosestKeyframe( const Eigen::Matrix4d& worldT ) const;

			void   selectVisibleFeatures( std::vector<size_t>	   & visibleFeatures,
									      std::vector<Vector2f>	   & projections,
										  const Eigen::Matrix4d	   & cameraPose,
										  const CameraCalibration  & camCalib );

			const MapFeature&		featureForId( size_t id ) const  { return _features[ id ];}
			const Keyframe&			keyframeForId( size_t id ) const { return _keyframes[ id ];}
			const Eigen::Matrix3d&  intrinsics() const { return _intrinsics; }

			size_t numFeatures()	 const { return _features.size(); }
			size_t numKeyframes()	 const { return _keyframes.size(); }
			size_t numMeasurements() const { return _numMeas; }

			void setIntrinsics( const Eigen::Matrix3d & K ) { _intrinsics = K; }

		private:
			std::vector<Keyframe>	_keyframes;
			std::vector<MapFeature>	_features;
			Eigen::Matrix3d			_intrinsics;
			size_t					_numMeas;
	};
}

#endif
