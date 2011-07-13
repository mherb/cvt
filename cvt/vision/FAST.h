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
		typedef void    (FAST::*ExtractFunc)( const uint8_t* im, size_t stride, size_t width, size_t height, std::vector<Feature2Df> & features );
		typedef int     (FAST::*ScoreFunc)( const uint8_t* p );

		public:
			FAST( FASTSize size = SEGMENT_9 );
			~FAST();

			void extract( const Image & image, std::vector<Feature2Df> & features );
			void extractMultiScale( const Image & image, std::vector<Feature2Df> & features, size_t octaves );
			void setThreshold( uint8_t threshold );
			uint8_t threshold();
			void setMinScore( int32_t minscore );
			int32_t minScore();

            void setNonMaxSuppress( bool val ) { _suppress = val; }

		private:
			uint8_t		_threshold;
			int32_t		_minScore;
			ExtractFunc	_extract;
			ScoreFunc	_score;
            bool        _suppress;
            size_t      _lastStride;
            int         _pixel[ 16 ]; // offsets to the ring pixel

			int* score( const uint8_t* img, size_t stride, std::vector<Feature2Df> & corners );

			void detect9( const uint8_t* im, size_t stride, size_t width, size_t height, std::vector<Feature2Df> & features );

            void detect9simd( const uint8_t* im, size_t stride, size_t width, size_t height, std::vector<Feature2Df> & features );

			int score9( const uint8_t* p );
			void detect10( const uint8_t* im, size_t stride, size_t width, size_t height, std::vector<Feature2Df> & features );
			int score10( const uint8_t* p );
			void detect11( const uint8_t* im, size_t stride, size_t width, size_t height, std::vector<Feature2Df> & features );
			int score11( const uint8_t* p );
			void detect12( const uint8_t* im, size_t stride, size_t width, size_t height, std::vector<Feature2Df> & features );
			int score12( const uint8_t* p );

			void make_offsets( size_t row_stride );
            void initPixelOffsets();
			void nonmaxSuppression( const std::vector<Feature2Df> & corners, const int* scores, std::vector<Feature2Df> & suppressed );

            bool isDarkerCorner9( const uint8_t * p, const int barrier );
            bool isBrighterCorner9( const uint8_t * p, const int barrier );
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
}

#endif
