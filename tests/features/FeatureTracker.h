#ifndef CVT_FEATURE_TRACKER_H
#define CVT_FEATURE_TRACKER_H

#include <cvt/vision/FAST.h>
#include <cvt/vision/Feature2D.h>

#include <cvt/util/ParamSet.h>
#include <cvt/vision/Patch.h>
#include <cvt/vision/IntegralImage.h>
#include "SegmentRing.h"

#include <vector>

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
					  std::vector<Feature2Df> & newFeatures );

			void setNccThreshold( float ncc ){ _nccThreshold = ncc; };
			void setCornerThreshold( int thresh );

		private:
			FAST                        _featureDetector;
            Image                       _lastImage;
            IntegralImage *             _currentIntegralImg;

            /* features in last Image represented by their patches */
            std::vector<Patch*>          _lastPatches;
            std::vector<Feature2Df>       _lastFeatures;

            int                 _threshold;
			float				_nccThreshold;
            int                 _searchRadius;

            float matchFeatureNCC( const Image & current, const Patch & patch, const Vector2f & startPos, Vector2f & out );
	};
}

#endif
