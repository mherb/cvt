#ifndef CVT_FEATURE_TRACKER_H
#define CVT_FEATURE_TRACKER_H

#include <cvt/vision/FeatureExtractor.h>
#include <cvt/util/ParamSet.h>
#include <cvt/vision/Patch.h>
#include <cvt/vision/IntegralImage.h>
#include "SegmentRing.h"

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
             *  newFeatures     the newly detected features
			 *
			 */
			void run( const Image & current,
					  std::vector<Feature2D> & newFeatures );


		private:
			FeatureExtractor<int32_t> * _featureDetector;
            Image                       _lastImage;
            IntegralImage *             _currentIntegralImg;

            /* features in last Image represented by their patches */
            std::vector<Patch*>          _lastPatches;

            int                     _threshold;
            int                  _searchRadius;
            
            float matchFeatureNCC( const Image & current, const Patch & patch, const Feature2D & startPos, Feature2D & out );

	};
}

#endif
