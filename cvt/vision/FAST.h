#ifndef CVT_FAST_H
#define CVT_FAST_H

#include <cvt/vision/Feature2D.h>
#include <cvt/util/CPU.h>
#include <cvt/util/Exception.h>
#include <cvt/gfx/Image.h>

#include <cvt/util/SIMDDebug.h>

#include <vector>

namespace cvt
{
	enum FASTSize {
		SEGMENT_9,
		SEGMENT_10,
		SEGMENT_11,
		SEGMENT_12
	};

    class ORB;

	class FAST
	{
		public:
			FAST( FASTSize size = SEGMENT_9 );
			~FAST();

            template<class PointContainer>
			void extract( const Image & image, PointContainer & features );

			void setThreshold( uint8_t threshold );
			uint8_t threshold();

            void setNonMaxSuppress( bool val ) { _suppress = val; }

            static void nonmaxSuppression( std::vector<Feature2Df> & suppressed, const std::vector<Feature2Df> & features );

            /*
             *  template PointContainer     Functor which has operator()( float x, float y )
             */
            template <class PointContainer>
            static void detect9( const Image & img, uint8_t threshold, PointContainer & features, size_t border = 3 );
            static void score9( const Image & img, std::vector<Feature2Df> & corners, uint8_t threshold );

            template <class PointContainer>
            static void detect10( const Image & img, uint8_t threshold, PointContainer & features, size_t border = 3 );
            static void score10( const Image & img, std::vector<Feature2Df> & corners, uint8_t threshold );

            template <class PointContainer>
            static void detect11( const Image & img, uint8_t threshold, PointContainer & features, size_t border = 3 );
            static void score11( const Image & img, std::vector<Feature2Df> & corners, uint8_t threshold );

            template <class PointContainer>
            static void detect12( const Image & img, uint8_t threshold, PointContainer & features, size_t border = 3 );
            static void score12( const Image & img, std::vector<Feature2Df> & corners, uint8_t threshold );

		private:
			uint8_t		_threshold;
            bool        _suppress;
            FASTSize    _fastSize;

            template<class PointContainer>
            void doExtract( const Image & img, PointContainer & features );

            static void make_offsets( int * offsets, size_t row_stride );

            static int  score9Pixel( const uint8_t* p, const int * offsets, uint8_t threshold );
            static int score10Pixel( const uint8_t* p, const int * offsets, uint8_t threshold );
            static int score11Pixel( const uint8_t* p, const int * offsets, uint8_t threshold );
            static int score12Pixel( const uint8_t* p, const int * offsets, uint8_t threshold );

            static bool isCorner9( const uint8_t * p, const int * offsets, uint8_t threshold );
            static bool isCorner10( const uint8_t * p, const int * offsets, uint8_t threshold );
            static bool isCorner11( const uint8_t * p, const int * offsets, uint8_t threshold );
            static bool isCorner12( const uint8_t * p, const int * offsets, uint8_t threshold );

            template<class PointContainer>
            static void detect9cpu( const Image & img, uint8_t threshold, PointContainer & features, size_t border = 3 );

            template<class PointContainer>
            static void detect9simd( const Image & img, uint8_t threshold, PointContainer & features, size_t border = 3 );
	};

	inline void FAST::setThreshold( uint8_t threshold )
	{
		_threshold = threshold;
	}

	inline uint8_t FAST::threshold()
	{
		return _threshold;
	}

	inline void FAST::make_offsets( int * offsets, size_t row_stride )
	{
        offsets[0]  =  0 + row_stride * 3;
		offsets[1]  =  1 + row_stride * 3;
		offsets[2]  =  2 + row_stride * 2;
		offsets[3]  =  3 + row_stride * 1;
		offsets[4]  =  3;
		offsets[5]  =  3 - row_stride * 1;
		offsets[6]  =  2 - row_stride * 2;
		offsets[7]  =  1 - row_stride * 3;
		offsets[8]  =    - row_stride * 3;
		offsets[9]  = -1 - row_stride * 3;
		offsets[10] = -2 - row_stride * 2;
		offsets[11] = -3 - row_stride * 1;
		offsets[12] = -3;
		offsets[13] = -3 + row_stride;
		offsets[14] = -2 + row_stride * 2;
		offsets[15] = -1 + row_stride * 3;
    }

    #include <cvt/vision/fast/FAST.inl>

}

#endif
