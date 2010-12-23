#ifndef CVT_FEATURE_EXTRACTOR_H
#define CVT_FEATURE_EXTRACTOR_H

#include <vector>

#include <cvt/gfx/Image.h>

#include <cvt/math/Vector.h>

namespace cvt {

	typedef Vector2i Feature2D;

	template <typename T>
	class FeatureExtractor
	{
		public:
			virtual ~FeatureExtractor(){};
			virtual void extract( const Image & image, std::vector<Vector2<T> > & features ) = 0;
			virtual void extractMultiScale( const Image & image, std::vector<Vector2<T> > & features, size_t octaves = 2 ) = 0;
	};

}

#endif
