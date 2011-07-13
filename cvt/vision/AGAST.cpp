#include <cvt/vision/AGAST.h>
#include <cvt/util/Exception.h>

#include <cvt/gfx/IScaleFilter.h>

namespace cvt
{

	AGAST::AGAST( ASTType type ) :
        _type( type ), _extract( 0 ), _score( 0 ), _threshold( 25 ), _lastStride( 0 ), _suppress( true )
	{
        switch ( type ) {
            case OAST_9_16:
                _extract = &AGAST::oast9_16;
                _score = &AGAST::score9_16;
                break;
            case AGAST_5_8:
                _extract = &AGAST::agast5_8;
                _score = &AGAST::score58;
                break;
            case AGAST_7_12D:
                _extract = &AGAST::agast7_12d;
                _score = &AGAST::score7_12d;
                break;
            case AGAST_7_12S:
                _extract = &AGAST::agast7_12s;
                _score = &AGAST::score7_12s;
                break;
            default:
                CVTException( "Unkown AGAST Type in initialization!" );
                break;
        }
	}

	AGAST::~AGAST()
	{
	}

	void AGAST::extract( const Image & image, std::vector<Feature2Df> & features )
	{
        size_t stride;
        const uint8_t * im = image.map( &stride );

        if( _suppress ){
            std::vector<Feature2Df> temp;
            ( this->*_extract )( im, stride, image.width(), image.height(), temp );
            int * scores = ( this->*_score )( im, stride, temp );
            nonMaximumSuppression( temp, scores, features );
            delete[] scores;
        } else {
            ( this->*_extract )( im, stride, image.width(), image.height(), features );
        }

        image.unmap( im );
	}


	void AGAST::extractMultiScale( const Image & image, std::vector<Feature2Df> & features, size_t octaves )
	{
		// construct the scale space
		std::vector<Image> pyramid;
		size_t width = image.width();
		size_t height = image.height();

		pyramid.resize( octaves - 1 );

		const Image * prevScale = &image;

		IScaleFilterGauss scaleFilter( 3.0f, 2.0f );
		for( size_t i = 0; i < pyramid.size(); i++ ){
			width >>= 1;
			height >>= 1;

			pyramid[ i ].reallocate( width, height, image.format() );
			prevScale->scale( pyramid[ i ], width, height, scaleFilter );

			prevScale = &pyramid[ i ];
		}

		this->extract( image, features );
		size_t previousScaleEnd = features.size();

		int32_t scale = 1;
		for( size_t i = 0; i < pyramid.size(); i++ ){
			scale <<= 1;

			this->extract( pyramid[ i ], features );
			while( previousScaleEnd < features.size() ){
				features[ previousScaleEnd ].pt *= scale;
				previousScaleEnd++;
			}
		}
	}

	void AGAST::nonMaximumSuppression(const std::vector<Feature2Df> & corners_all, const int* scores, std::vector<Feature2Df> & nms )
	{
		size_t numCorners_all = corners_all.size();
		int nmsFlags[ numCorners_all ]; //-1 max, else index to max
		int lastRow=0, next_lastRow=0;

		std::vector<Feature2Df>::const_iterator currentCorner = corners_all.begin();
		std::vector<Feature2Df>::const_iterator cornersEnd = corners_all.end();

		size_t lastRowCorner_ind=0, next_lastRowCorner_ind=0;
		int *nmsFlags_p=nmsFlags;

		nms.reserve( numCorners_all );

		//set all flags to MAXIMUM
		int j;
		for(j=numCorners_all; j>0; j--)
			*nmsFlags_p++=-1;
		nmsFlags_p = nmsFlags;

		for( size_t currCorner_ind=0; currCorner_ind < numCorners_all; currCorner_ind++ )
		{
			//check above
			if( lastRow + 1 < currentCorner->pt.y )
			{
				lastRow=next_lastRow;
				lastRowCorner_ind=next_lastRowCorner_ind;
			}
			if( next_lastRow != currentCorner->pt.y )
			{
				next_lastRow=currentCorner->pt.y;
				next_lastRowCorner_ind=currCorner_ind;
			}
			if( lastRow+1==currentCorner->pt.y )
			{
				//find the corner above the current one
				while( ( corners_all[lastRowCorner_ind].pt.x < currentCorner->pt.x ) &&
                       ( corners_all[lastRowCorner_ind].pt.y == lastRow ) )
					lastRowCorner_ind++;

				if( ( corners_all[ lastRowCorner_ind ].pt.x == currentCorner->pt.x ) &&
                    ( lastRowCorner_ind != currCorner_ind ) )
				{
					int t=lastRowCorner_ind;
					while( nmsFlags[ t ] != -1 ) //find the maximum in this block
						t = nmsFlags[ t ];

					if( scores[currCorner_ind] < scores[t] )
					{
						nmsFlags[currCorner_ind]=t;
					}
					else
						nmsFlags[t]=currCorner_ind;
				}
			}

			//check left
			size_t t = currCorner_ind - 1;
			if( ( currCorner_ind != 0 ) &&
                ( corners_all[ t ].pt.y == currentCorner->pt.y ) &&
                ( corners_all[ t ].pt.x + 1 == currentCorner->pt.x ) )
			{
				int currCornerMaxAbove_ind = nmsFlags[ currCorner_ind ];

				while( nmsFlags[ t ]!=-1) //find the maximum in that area
					t=nmsFlags[ t ];

				if( currCornerMaxAbove_ind==-1 ) //no maximum above
				{
					if(t!=currCorner_ind)
					{
						if( scores[currCorner_ind] < scores[t] )
							nmsFlags[currCorner_ind]=t;
						else
							nmsFlags[t]=currCorner_ind;
					}
				}
				else	//maximum above
				{
					if( (int)t != currCornerMaxAbove_ind )
					{
						if( scores[currCornerMaxAbove_ind] < scores[t])
						{
							nmsFlags[currCornerMaxAbove_ind]=t;
							nmsFlags[currCorner_ind]=t;
						}
						else
						{
							nmsFlags[ t ]=currCornerMaxAbove_ind;
							nmsFlags[ currCorner_ind ]=currCornerMaxAbove_ind;
						}
					}
				}
			}

			currentCorner++;
		}

		//collecting maximum corners
		for( size_t currCorner_ind = 0; currCorner_ind < numCorners_all; currCorner_ind++ )
		{
			if( *nmsFlags_p++ == -1 ){
				nms.push_back( Feature2Df( corners_all[ currCorner_ind ] ) );
			}
		}
	}

	void AGAST::init9_16_pattern( size_t stride )
	{
		if( stride == _lastStride )
			return;

		_lastStride = stride;

		_offset0= ( -3 );
		_offset1=(-3)+(-1)*_lastStride;
		_offset2=(-2)+(-2)*_lastStride;
		_offset3=(-1)+(-3)*_lastStride;
		_offset4=(0)+(-3)*_lastStride;
		_offset5=(1)+(-3)*_lastStride;
		_offset6=(2)+(-2)*_lastStride;
		_offset7=(3)+(-1)*_lastStride;
		_offset8=(3)+(0)*_lastStride;
		_offset9=(3)+(1)*_lastStride;
		_offset10=(2)+(2)*_lastStride;
		_offset11=(1)+(3)*_lastStride;
		_offset12=(0)+(3)*_lastStride;
		_offset13=(-1)+(3)*_lastStride;
		_offset14=(-2)+(2)*_lastStride;
		_offset15=(-3)+(1)*_lastStride;
	}
}
