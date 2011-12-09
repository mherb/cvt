#ifndef CVT_SLAMMAP_H
#define CVT_SLAMMAP_H

#include <cvt/vision/Keyframe.h>
#include <cvt/vision/MapFeature.h>

namespace cvt
{
	class SlamMap
	{
		public:
			SlamMap();
			~SlamMap();

			size_t addKeyframe( const Image & img );

			void addFeatureToKeyframe( const Vector4d & world,
									   const ORBFeature & orbfeature,
									   size_t keyframeId );

		private:
			std::vector<Keyframe>	_keyframes;
			std::Vector<MapFeature>	_features;

			// TODO: add a Keyframe-Graph here?
	};
}

#endif
