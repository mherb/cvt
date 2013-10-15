#ifndef CVT_FEATUREDETECTOR_H
#define CVT_FEATUREDETECTOR_H

#include <cvt/gfx/Image.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/vision/features/FeatureSet.h>

namespace cvt {

	class FeatureDetector
	{
		public:
			virtual ~FeatureDetector() {}
			virtual void detect( FeatureSet& set, const Image& img ) = 0;
			virtual void detect( FeatureSet& set, const ImagePyramid& imgpyr) = 0;

			virtual void setBorder( size_t border ) = 0;
			virtual size_t border() const = 0;


	};
}

#endif
