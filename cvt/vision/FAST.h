#ifndef CVT_FAST_H
#define CVT_FAST_H

#include <cvt/vision/Feature2D.h>
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

    class ORB;

	class FAST
	{
		typedef void (*ExtractFunc)( const Image & im, uint8_t threshold, std::vector<Feature2Df> & features, size_t border );
		typedef int  (*ScoreFunc)( const uint8_t* p, const int * offsets, uint8_t threshold );

		public:
			FAST( FASTSize size = SEGMENT_9 );
			~FAST();

			void extract( const Image & image, std::vector<Feature2Df> & features );

			void setThreshold( uint8_t threshold );
			uint8_t threshold();
			void setMinScore( int32_t minscore );
			int32_t minScore();

            void setNonMaxSuppress( bool val ) { _suppress = val; }

            static void nonmaxSuppression( const std::vector<Feature2Df> & corners, const int* scores, std::vector<Feature2Df> & suppressed );

		private:
			uint8_t		_threshold;
			int32_t		_minScore;
			ExtractFunc	_extract;
			ScoreFunc	_score;
            bool        _suppress;

			int* score( const uint8_t* img, size_t stride, std::vector<Feature2Df> & corners );
			
            static void detect9( const Image & img, uint8_t threshold, std::vector<Feature2Df> & features, size_t border = 3 );
            static void detect9simd( const Image & img, uint8_t threshold, std::vector<Feature2Df> & features, size_t border = 3 );
            static int score9( const uint8_t* p, const int * offsets, uint8_t threshold );
            static bool isDarkerCorner9( const uint8_t * p, const int barrier, const int * offsets );
            static bool isBrighterCorner9( const uint8_t * p, const int barrier, const int * offsets );
        
			static void detect10( const Image & img, uint8_t threshold, std::vector<Feature2Df> & features, size_t border = 3 );
			static int score10( const uint8_t* p, const int * offsets, uint8_t threshold );
        
			static void detect11( const Image & img, uint8_t threshold, std::vector<Feature2Df> & features, size_t border = 3 );
			static int score11( const uint8_t* p, const int * offsets, uint8_t threshold );
        
			static void detect12( const Image & img, uint8_t threshold, std::vector<Feature2Df> & features, size_t border = 3 );
			static int score12( const uint8_t* p, const int * offsets, uint8_t threshold );

            static void make_offsets( int * offsets, size_t row_stride );
	};

	inline void FAST::setThreshold( uint8_t threshold )
	{
		_threshold = threshold;
	}

	inline uint8_t FAST::threshold()
	{
		return _threshold;
	}

	inline void FAST::setMinScore( int32_t minscore )
	{
		_minScore = minscore;
	}

	inline int32_t FAST::minScore()
	{
		return _minScore;
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
}

#endif
