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
					  const Image & last,
					  const std::vector<Feature2D> & predicted,
					  std::vector<Feature2D> & trackedPoints,
					  std::vector<size_t> & indices,
					  std::vector<Feature2D> & newFeatures );

		private:
			FeatureExtractor<int32_t> * _featureDetector;

			float patchDistance( const Image & im0, const Image & im1,
								 const Feature2D & pos0, const Feature2D & pos1 );
			float matchPatch( const Image & im0, const Image & im1,
							  const Feature2D & pos0, const Feature2D & pos1 );
	};
}

#endif
