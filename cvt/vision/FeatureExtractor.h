#ifndef CVT_FEATURE_EXTRACTOR_H
#define CVT_FEATURE_EXTRACTOR_H

#include <vector>

#include <cvt/gfx/Image.h>

namespace cvt {

	template <typename T, size_t dimension = 2>
	struct Feature
	{
		Feature()
		{
			memset( coords, 0, sizeof( T ) * dimension );
		}

		Feature( const Feature & other )
		{
			memcpy( this->coords, other.coords, sizeof( T ) * dimension );
		}

		~Feature()
		{}

		T coords[ dimension ];

		const T & operator[]( size_t index ) const { return coords[ index ]; }
		T & operator[]( size_t index ) { return coords[ index ]; }
	};

	typedef Feature<int32_t, 2> Feature2D;

	template <typename T, size_t FeatureDimension = 2>
	class FeatureExtractor
	{
		public:
			virtual ~FeatureExtractor(){};
			virtual void extract( const Image & image, std::vector<Feature<T, FeatureDimension> > & features ) = 0;
	};

}

#endif
