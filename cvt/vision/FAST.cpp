#include <cvt/vision/FAST.h>
#include <cvt/util/Exception.h>
#include <cvt/gfx/IScaleFilter.h>

namespace cvt
{

#define Compare(X, Y) ((X)>=(Y))

	FAST::FAST( FASTSize size ) :
		_threshold( 30 ),
		_minScore( 1 ),
		_extract( 0 ),
		_score( 0 ),
        _suppress( true ),
        _lastStride( 640 )
	{
		switch( size ){
			case SEGMENT_9:
				//_extract = &FAST::detect9simd;
                _extract = &FAST::detect9;
				_score = &FAST::score9;
				break;
			case SEGMENT_10:
				_extract = &FAST::detect10;
				_score = &FAST::score10;
				break;
			case SEGMENT_11:
				_extract = &FAST::detect11;
				_score = &FAST::score11;
				break;
			case SEGMENT_12:
				_extract = &FAST::detect12;
				_score = &FAST::score12;
				break;
			default:
				throw CVTException( "NO MATCHING EXTRACT FUNC" );
				break;
		}
        
        initPixelOffsets();
	}

	FAST::~FAST()
	{
	}

	void FAST::extract( const Image & image, std::vector<Feature2D> & features )
	{
		size_t stride;

		if( image.format() != IFormat::GRAY_UINT8 )
			throw CVTException( "Input Image format must be GRAY_UINT8" );

		const uint8_t* im = image.map( &stride );		

        if( _suppress ){
            std::vector<Feature2D> allCorners;
            // detect candidates
            (this->*_extract)( im, stride, image.width(), image.height(), allCorners );
            
            // calc the scores
            int * cornerScores = this->score( im, stride, allCorners );		
            // non maximal suppression
            this->nonmaxSuppression( allCorners, cornerScores, features );
        } else { 
            (this->*_extract)( im, stride, image.width(), image.height(), features );
        }
        
        image.unmap( im );
	}


	void FAST::extractMultiScale( const Image & image, std::vector<Feature2D> & features, size_t octaves )
	{
		// construct the scale space
		std::vector<Image> pyramid;
		size_t width = image.width();
		size_t height = image.height();

		IScaleFilterGauss scaleFilter( 2.0f, 0.0f );
		
		pyramid.resize( octaves - 1 );

		const Image * prevScale = &image;

		for( size_t i = 0; i < pyramid.size(); i++ ){
			width >>= 1;
			height >>= 1;

			pyramid[ i ].reallocate( width, height, image.format() );
			prevScale->scale( pyramid[ i  ], width, height, scaleFilter );
			prevScale = &pyramid[ i ];			
		}

		this->extract( image, features );
		size_t previousScaleEnd = features.size();

		int32_t scale = 1;
		for( size_t i = 0; i < pyramid.size(); i++ ){
			scale <<= 1;

			this->extract( pyramid[ i ], features );
			while( previousScaleEnd < features.size() ){
				features[ previousScaleEnd ]*=scale;
				previousScaleEnd++;
			}
		}
	}

	void FAST::make_offsets( size_t row_stride )
	{
        
        if( _lastStride == row_stride )
            return;
        
        _lastStride = row_stride;
        initPixelOffsets();
	}
    
    void FAST::initPixelOffsets()
    {
        pixel[0]  =  0 + _lastStride * 3;
		pixel[1]  =  1 + _lastStride * 3;
		pixel[2]  =  2 + _lastStride * 2;
		pixel[3]  =  3 + _lastStride * 1;
		pixel[4]  =  3;
		pixel[5]  =  3 - _lastStride * 1;
		pixel[6]  =  2 - _lastStride * 2;
		pixel[7]  =  1 - _lastStride * 3;
		pixel[8]  =    - _lastStride * 3;
		pixel[9]  = -1 - _lastStride * 3;
		pixel[10] = -2 - _lastStride * 2;
		pixel[11] = -3 - _lastStride * 1;
		pixel[12] = -3;
		pixel[13] = -3 + _lastStride;
		pixel[14] = -2 + _lastStride * 2;
		pixel[15] = -1 + _lastStride * 3;
    }

	void FAST::nonmaxSuppression( const std::vector<Feature2D> & corners, const int* scores, std::vector<Feature2D> & suppressed )
	{

		/* Point above points (roughly) to the pixel above the one of interest, if there
		   is a feature there. */
		int point_above = 0;
		int point_below = 0;

		size_t numCorners = corners.size();
		if( numCorners < 1 ) {
			return;
		}

		/* Find where each row begins
		   (the corners are output in raster scan order). A beginning of -1 signifies
		   that there are no corners on that row. */
		int32_t last_row;
		last_row = corners[ numCorners - 1 ][ 1 ];
		int* rowStart;
		rowStart = ( int* )malloc( ( last_row + 1 ) * sizeof( int ) );

		for( int i = 0; i < last_row + 1; i++ ){
			rowStart[ i ] = -1;
		}

		{
			int prev_row = -1;
			for( size_t i = 0; i < numCorners; i++ ){
				if( (int)corners[ i ][ 1 ] != prev_row ) {
					rowStart[ corners[ i ][ 1 ] ] = i;
					prev_row = corners[ i ][ 1 ];
				}
			}
		}

		for( size_t i = 0; i < numCorners; i++ ) {
			int score = scores[ i ];
			if( score < _minScore )
				continue;
			const Feature2D & pos = corners[ i ];

			/* Check left */
			if( i > 0 )
				if( corners[ i - 1 ][ 0 ] == pos[ 0 ] - 1 && corners[ i - 1 ][ 1 ] == pos[ 1 ] && Compare( scores[ i - 1 ], score ) )
					continue;

			/* Check right */
			if( i < ( numCorners - 1 ) )
				if( corners[ i + 1 ][ 0 ] == pos[ 0 ] + 1 && corners[ i + 1 ][ 1 ] == pos[ 1 ] && Compare( scores[ i + 1 ], score ) )
					continue;

			/* Check above (if there is a valid row above) */
			if( pos[ 1 ] != 0 && rowStart[ pos[ 1 ] - 1 ] != -1 ){
				/* Make sure that current point_above is one row above. */
				if( corners[ point_above ][ 1 ] < pos[ 1 ] - 1 )
					point_above = rowStart[ pos[ 1 ] - 1 ];

				/* Make point_above point to the first of the pixels above the current point, if it exists. */
				for(; corners[ point_above ][ 1 ] < pos[ 1 ] && corners[ point_above ][ 0 ] < pos[ 0 ] - 1; point_above++ )
				{}


				for( size_t j = point_above; corners[ j ][ 1 ] < pos[ 1 ] && corners[ j ][ 0 ] <= pos[ 0 ] + 1; j++ ){
					int x = corners[ j ][ 0 ];
					if( ( x == pos[ 0 ] - 1 || x ==pos[ 0 ] || x == pos[ 0 ] + 1 ) && Compare( scores[ j ], score ) )
						goto cont;
				}

			}

			/* Check below (if there is anything below) */
			if( pos[ 1 ] != last_row && rowStart[ pos[ 1 ] + 1 ] != -1 && point_below < (int)numCorners ) /*Nothing below*/ {
				if( corners[ point_below ][ 1 ] < pos[ 1 ] + 1 )
					point_below = rowStart[ pos[ 1 ]+1 ];

				/* Make point below point to one of the pixels below the current point, if it exists. */
				for(; point_below < (int)numCorners && corners[ point_below ][ 1 ] == pos[ 1 ] + 1 && corners[ point_below ][ 0 ] < pos[ 0 ] - 1; point_below++ )
				{}

				for( size_t j=point_below; j < numCorners && corners[ j ][ 1 ] == pos[ 1 ] + 1 && corners[ j ][ 0 ] <= pos[ 0 ] + 1; j++ ) {
					int x = corners[ j ][ 0 ];
					if( ( x == pos[ 0 ] - 1 || x == pos[ 0 ] || x == pos[ 0 ] + 1 ) && Compare( scores[ j ], score ) )
						goto cont;
				}
			}

			suppressed.push_back( corners[ i ] );

cont:
			;
		}

		free( rowStart );
	}

	/* calc the scores for all the corners */
	int* FAST::score(const uint8_t* img, size_t stride, std::vector<Feature2D> & corners )
	{
		int* scores = new int[ corners.size() ];

		for( size_t n = 0; n < corners.size(); n++ )
			scores[ n ] = (this->*_score)( img + corners[ n ][ 1 ] * stride + corners[ n ][ 0 ] );

		return scores;
	}

}
