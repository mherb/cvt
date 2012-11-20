/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_FAST_H
#define CVT_FAST_H

#include <cvt/vision/features/FeatureDetector.h>
#include <cvt/util/CPU.h>
#include <cvt/util/Exception.h>
#include <cvt/gfx/Image.h>

#include <vector>

namespace cvt
{
	enum FASTSize {
		SEGMENT_9,
		SEGMENT_10,
		SEGMENT_11,
		SEGMENT_12
	};

	class FAST : public FeatureDetector
	{
		public:
			FAST( FASTSize size = SEGMENT_9, uint8_t threshold = 30, size_t border = 3 );
			~FAST();

			void detect( const Image& image, FeatureSet& features );
			void detect( const ImagePyramid& image, FeatureSet& features );

			void setThreshold( uint8_t threshold )	{ _threshold = threshold; }
			uint8_t threshold() const				{ return _threshold; }

			void setBorder( size_t border )			{ _border = border; }
			size_t border() const					{ return _border; }

		private:
            FASTSize    _fastSize;
			uint8_t		_threshold;
            size_t		_border;

            static void make_offsets( int * offsets, size_t row_stride );

            static void detect9( const Image & img, uint8_t threshold, FeatureSet& features, size_t border = 3 );
            static void score9( const Image & img, FeatureSet& corners, uint8_t threshold );

            static void detect10( const Image & img, uint8_t threshold, FeatureSet& features, size_t border = 3 );
            static void score10( const Image & img, FeatureSet& corners, uint8_t threshold );

            static void detect11( const Image & img, uint8_t threshold, FeatureSet& features, size_t border = 3 );
            static void score11( const Image & img, FeatureSet& corners, uint8_t threshold );

            static void detect12( const Image & img, uint8_t threshold, FeatureSet& features, size_t border = 3 );
            static void score12( const Image & img, FeatureSet& corners, uint8_t threshold );

            static int score9Pixel( const uint8_t* p, const int * offsets, uint8_t threshold );
            static int score10Pixel( const uint8_t* p, const int * offsets, uint8_t threshold );
            static int score11Pixel( const uint8_t* p, const int * offsets, uint8_t threshold );
            static int score12Pixel( const uint8_t* p, const int * offsets, uint8_t threshold );

            static bool isCorner9( const uint8_t * p, const int * offsets, uint8_t threshold );
            static bool isCorner10( const uint8_t * p, const int * offsets, uint8_t threshold );
            static bool isCorner11( const uint8_t * p, const int * offsets, uint8_t threshold );
            static bool isCorner12( const uint8_t * p, const int * offsets, uint8_t threshold );

            static void detect9cpu( const Image & img, uint8_t threshold, FeatureSet& features, size_t border = 3 );
            static void detect9simd( const Image & img, uint8_t threshold, FeatureSet& features, size_t border = 3 );
	};

	inline void FAST::make_offsets( int* offsets, size_t row_stride )
	{
        offsets[ 0  ] =  0 + row_stride * 3;
		offsets[ 1  ] =  1 + row_stride * 3;
		offsets[ 2  ] =  2 + row_stride * 2;
		offsets[ 3  ] =  3 + row_stride * 1;
		offsets[ 4  ] =  3;
		offsets[ 5  ] =  3 - row_stride * 1;
		offsets[ 6  ] =  2 - row_stride * 2;
		offsets[ 7  ] =  1 - row_stride * 3;
		offsets[ 8  ] =    - row_stride * 3;
		offsets[ 9  ] = -1 - row_stride * 3;
		offsets[ 10 ] = -2 - row_stride * 2;
		offsets[ 11 ] = -3 - row_stride * 1;
		offsets[ 12 ] = -3;
		offsets[ 13 ] = -3 + row_stride;
		offsets[ 14 ] = -2 + row_stride * 2;
		offsets[ 15 ] = -1 + row_stride * 3;
    }

    #include <cvt/vision/fast/FAST.inl>

}

#endif
