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

			size_t addKeyframe( const Image& img, const Eigen::Matrix4d& pose );

			void addFeatureToKeyframe( const MapFeature& world,
									   const ORBFeature& orbfeature,
									   size_t keyframeId );

			size_t findClosestKeyframe( const Eigen::Matrix4d& worldT ) const;

			void   selectVisibleFeatures( std::vector<MapFeature*> & visibleFeatures,
									      std::vector<Vector2f>	   & projections,
										  const Eigen::Matrix4d	   & cameraPose,
										  const CameraCalibration  & camCalib );

		private:
			std::vector<Keyframe>	_keyframes;
			std::vector<MapFeature>	_features;

			// TODO: add a Keyframe-Graph here?
	};
}

#endif
