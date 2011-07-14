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
        _suppress( true )
	{
		switch( size ){
			case SEGMENT_9:
				_extract = &detect9simd;
                //_extract = &FAST::detect9;
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
	}

	FAST::~FAST()
	{
	}

	void FAST::extract( const Image & image, std::vector<Feature2Df> & features )
	{
		size_t stride;

		if( image.format() != IFormat::GRAY_UINT8 )
			throw CVTException( "Input Image format must be GRAY_UINT8" );

		const uint8_t* im = image.map( &stride );

        if( _suppress ){
            std::vector<Feature2Df> allCorners;
            // detect candidates
            _extract( image, _threshold, allCorners, 3 );

            // calc the scores
            int * cornerScores = this->score( im, stride, allCorners );
            // non maximal suppression
            this->nonmaxSuppression( allCorners, cornerScores, features );
        } else {
            _extract( image, _threshold, features, 3 );
        }

        image.unmap( im );
	}

	void FAST::nonmaxSuppression( const std::vector<Feature2Df> & corners, const int* scores, std::vector<Feature2Df> & suppressed )
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
		last_row = corners[ numCorners - 1 ].pt.y;
		int* rowStart;
		rowStart = ( int* )malloc( ( last_row + 1 ) * sizeof( int ) );

		for( int i = 0; i < last_row + 1; i++ ){
			rowStart[ i ] = -1;
		}

		{
			int prev_row = -1;
			for( size_t i = 0; i < numCorners; i++ ){
				if( (int)corners[ i ].pt.y != prev_row ) {
					rowStart[ (int)corners[ i ].pt.y ] = i;
					prev_row = corners[ i ].pt.y;
				}
			}
		}

		for( size_t i = 0; i < numCorners; i++ ) {
			int score = scores[ i ];

			const Vector2f & pos = corners[ i ].pt;

			/* Check left */
			if( i > 0 )
				if( corners[ i - 1 ].pt.x == pos[ 0 ] - 1 && corners[ i - 1 ].pt.y == pos[ 1 ] && Compare( scores[ i - 1 ], score ) )
					continue;

			/* Check right */
			if( i < ( numCorners - 1 ) )
				if( corners[ i + 1 ].pt.x == pos[ 0 ] + 1 && corners[ i + 1 ].pt.y == pos[ 1 ] && Compare( scores[ i + 1 ], score ) )
					continue;

			/* Check above (if there is a valid row above) */
			if( pos[ 1 ] != 0 && rowStart[ (int)pos[ 1 ] - 1 ] != -1 ){
				/* Make sure that current point_above is one row above. */
				if( corners[ point_above ].pt.y < pos[ 1 ] - 1 )
					point_above = rowStart[ (int)pos[ 1 ] - 1 ];

				/* Make point_above point to the first of the pixels above the current point, if it exists. */
				for(; corners[ point_above ].pt.y < pos[ 1 ] && corners[ point_above ].pt.x < pos[ 0 ] - 1; point_above++ )
				{}


				for( size_t j = point_above; corners[ j ].pt.y < pos[ 1 ] && corners[ j ].pt.x <= pos[ 0 ] + 1; j++ ){
					int x = corners[ j ].pt.x;
					if( ( x == pos[ 0 ] - 1 || x == pos[ 0 ] || x == pos[ 0 ] + 1 ) && Compare( scores[ j ], score ) )
						goto cont;
				}

			}

			/* Check below (if there is anything below) */
			if( pos[ 1 ] != last_row && rowStart[ (int)pos[ 1 ] + 1 ] != -1 && point_below < (int)numCorners ) /*Nothing below*/ {
				if( corners[ point_below ].pt.y < pos[ 1 ] + 1 )
					point_below = rowStart[ (int)pos[ 1 ]+1 ];

				/* Make point below point to one of the pixels below the current point, if it exists. */
				for(; point_below < (int)numCorners && corners[ point_below ].pt.y == pos[ 1 ] + 1 && corners[ point_below ].pt.x < pos[ 0 ] - 1; point_below++ )
				{}

				for( size_t j=point_below; j < numCorners && corners[ j ].pt.y == pos[ 1 ] + 1 && corners[ j ].pt.x <= pos[ 0 ] + 1; j++ ) {
					int x = corners[ j ].pt.x;
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
	int* FAST::score(const uint8_t* img, size_t stride, std::vector<Feature2Df> & corners )
	{
        int offsets[ 16 ];
        make_offsets( offsets, stride );
		int* scores = new int[ corners.size() ];

		for( size_t n = 0; n < corners.size(); n++ )
			scores[ n ] = _score( img + (int)corners[ n ].pt.y * stride + (int)corners[ n ].pt.x, offsets, _threshold );

		return scores;
	}

}
