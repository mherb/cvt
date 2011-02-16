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
             *  newFeatures     the newly detected features
			 *	
			 */
			void run( const Image & current,
					  std::vector<Feature2D> & newFeatures );

		private:
			FeatureExtractor<int32_t> * _featureDetector;        
            Image                       _lastImage;
        
            /* features in last Image */
            std::vector<Feature2D>  _lastBrighter;
            std::vector<Feature2D>  _lastDarker;
            size_t                  _numLast;
        
            std::vector<Feature2D>  _currentBrighter;
            std::vector<Feature2D>  _currentDarker;
        
            void partitionFeatures( const std::vector<Feature2D> & features, 
                                    std::vector<Feature2D> & b,
                                    std::vector<Feature2D> & d );
	};
}

#endif
