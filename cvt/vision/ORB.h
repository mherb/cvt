#ifndef CVT_ORB_H
#define CVT_ORB_H

#include <cvt/vision/Feature2D.h>
#include <cvt/gfx/Image.h>
#include <cvt/vision/IntegralImage.h>
#include <vector>

namespace cvt {

	struct ORBFeature : Feature2Df {
		uint8_t desc[ 32 ]; // 256 bit vector
	};

	class ORB {
		public:
			ORB( const Image& img, size_t octaves = 3, float scalefactor = 0.5f );

			size_t size() const;
			const ORBFeature& operator[]( size_t index ) const;

		private:
			void detect( const Image& img, float scale );
			void centroidAngle( ORBFeature& feature, IntegralImage& iimg  );
			void descriptor( ORBFeature& feature, IntegralImage& iimg );

			std::vector<ORBFeature> _features;
			static int _patterns[ 30 ][ 512 ][ 2 ];
	};

	inline size_t ORB::size() const
	{
		return _features.size();
	}

	inline const ORBFeature& ORB::operator[]( size_t index ) const
	{
		return _features[ index ];
	}
}

#endif
