#ifndef CVT_FEATURE_TRACKER_H
#define CVT_FEATURE_TRACKER_H

#include <cvt/vision/FeatureExtractor.h>

namespace cvt
{
	class FeatureTracker
	{
		public:
			FeatureTracker();
			~FeatureTracker();

			/**
			 *	try to rematch lastPoints with the actual image
			 *	current:		the actual frame
			 *	last:			the last frame
			 *	lastPoints:		features tracked in last frame
			 * currentPoints:	points currently tracked
			 *	indices:		corresponding indices
			 */
			void run( const Image & current,
					  std::vector<Feature2D> & newFeatures );

		private:
			FeatureExtractor<int32_t> * _featureDetector;			
	};
}

#endif
