#ifndef CVT_AGAST_H
#define CVT_AGAST_H

#include <cvt/vision/FeatureExtractor.h>
#include <cvt/gfx/Image.h>

namespace cvt {

	enum ASTType {
		OAST_9_16,
		AGAST_5_8,
		AGAST_7_12D,
		AGAST_7_12S
	};

	class AGAST : public FeatureExtractor<int32_t>
	{
		public:
            typedef void (AGAST::*ExtractFunc)( const uint8_t* im, size_t stride, size_t width, size_t height, std::vector<Feature2D> & features );
		typedef int* (AGAST::*ScoreFunc)( const uint8_t* p, size_t stride, std::vector<Feature2D> & corner );
			AGAST( ASTType type = AGAST_5_8 );
			~AGAST();
            
			void extract( const Image & image, std::vector<Feature2D> & features );
			void extractMultiScale( const Image & image, std::vector<Feature2D> & features, size_t octaves );
            void setNonMaxSuppress( bool val ){ _suppress = val; }
            void setThreshold( int val ){ _threshold = val; }

		private:
			ASTType		_type;
            ExtractFunc	_extract;
            ScoreFunc	_score;
			int			_threshold;
			size_t		_lastStride;
            bool        _suppress;
			int16_t		_offset0;
			int16_t		_offset1;
			int16_t		_offset2;
			int16_t		_offset3;
			int16_t		_offset4;
			int16_t		_offset5;
			int16_t		_offset6;
			int16_t		_offset7;
			int16_t		_offset8;
			int16_t		_offset9;
			int16_t		_offset10;
			int16_t		_offset11;
			int16_t		_offset12;
			int16_t		_offset13;
			int16_t		_offset14;
			int16_t		_offset15;


			void oast9_16( const uint8_t* im, size_t stride, size_t xsize, size_t ysize, std::vector<Feature2D> & corners );
			void agast5_8( const uint8_t * im, size_t stride, size_t xsize, size_t ysize, std::vector<Feature2D> & corners );
			void agast7_12s( const uint8_t * im, size_t stride, size_t xsize, size_t ysize, std::vector<Feature2D> & corners );
			void agast7_12d( const uint8_t * im, size_t stride, size_t xsize, size_t ysize, std::vector<Feature2D> & corners );


			void init9_16_pattern( size_t stride );
			void init5_8_pattern( size_t stride );
			void init7_12s_pattern( size_t stired );
			void init7_12d_pattern( size_t stired );

			int score_oast9_16( const uint8_t * p );
			int score_agast5_8( const uint8_t * p );
			int score_agast7_12s( const uint8_t * p );
			int score_agast7_12d( const uint8_t * p );

			int* score9_16( const uint8_t * i, size_t stride, std::vector<Feature2D> & corners );
			int* score58( const uint8_t * i, size_t stride, std::vector<Feature2D> & corners );
			int* score7_12s( const uint8_t* i, size_t stride, std::vector<Feature2D> & corners );
			int* score7_12d( const uint8_t* i, size_t stride, std::vector<Feature2D> & corners );

			void nonMaximumSuppression( const std::vector<Feature2D> & corners_all,
									    const int* scores,
										std::vector<Feature2D> & nms );

	};
}

#endif
