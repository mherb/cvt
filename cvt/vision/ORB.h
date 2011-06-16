#ifndef CVT_ORB_H
#define CVT_ORB_H

#include <cvt/vision/Feature2D.h>
#include <cvt/gfx/Image.h>
#include <cvt/vision/IntegralImage.h>
#include <cvt/util/SIMD.h>
#include <vector>

namespace cvt {

	struct ORBFeature : public Feature2Df {
        ORBFeature( float x, float y, float angle = 0.0f, float scale = 1.0f ) : 
            Feature2Df( x, y, angle, scale )
        {
        }

		size_t distance( const ORBFeature& f ) const
		{
			return SIMD::instance()->hammingDistance( ( const uint64_t* ) desc, ( const uint64_t* ) f.desc, 4 );
		}

		uint8_t desc[ 32 ]; // 256 bit vector
	};

	class ORB {
		public:
			ORB( const Image& img, size_t octaves = 3, float scalefactor = 0.5f, uint8_t cornerThreshold = 25 );

			size_t size() const;
			const ORBFeature& operator[]( size_t index ) const;

		private:
			void detect( const Image& img, float scale );
			void centroidAngle( ORBFeature& feature, const float* ptr, size_t widthstep );
			void descriptor( ORBFeature& feature, const float* ptr, size_t widthstep );

            void detect9( const uint8_t* im, size_t stride, size_t width, size_t height, float scale );
            void makeOffsets( size_t stride );
            bool isDarkerCorner9( const uint8_t * p, const int barrier );
            bool isBrighterCorner9( const uint8_t * p, const int barrier );

			std::vector<ORBFeature> _features;

            // for OFAST
            uint8_t			 _threshold;
            int				 _pixel[ 16 ];// for OFAST

			static int       _patterns[ 30 ][ 512 ][ 2 ];
			static int		 _circularoffset[ 31 ];
            static const int _halfPatchSize = 15;
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
