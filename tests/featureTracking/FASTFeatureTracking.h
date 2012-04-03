#ifndef CVT_FAST_FEATURE_TRACKING_H
#define CVT_FAST_FEATURE_TRACKING_H

#include <cvt/vision/FAST.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/vision/FeatureFilter.h>
#include <cvt/vision/KLTTracker.h>
#include <cvt/math/GA2.h>

#include <vector>
#include <set>

namespace cvt
{
	class FASTFeatureTracking
	{
		public:
			static const size_t PatchSize = 8;
			typedef GA2<float>							PoseType;
			typedef KLTTracker<PoseType, PatchSize>		KLTType;	
			typedef typename KLTType::KLTPType			PatchType;
			
			FASTFeatureTracking();
			~FASTFeatureTracking();

			/**
			 *	\brief try to track features in current image
			 *	\param predicted	predicted visible patches (including positions)
			 */
			void trackFeatures( std::vector<PatchType*>& tracked, 
							    std::vector<PatchType*>& predicted,
							    const Image& image );

		private:
			FAST					_detector;
			ImagePyramid			_pyramid;

			KLTType					_klt;

			size_t					_fastMatchingWindowSqr;
			float					_fastMinMatchingThreshold;

			std::vector<Feature2Df>	_currentFeatures;
			std::set<size_t>		_associatedIndexes;

			void detectCurrentFeatures( const Image& img );

			int bestFASTFeatureInRadius( const Image& img, const PatchType& patch );

	};
}

#endif
