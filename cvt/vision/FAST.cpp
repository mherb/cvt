#include <cvt/vision/FAST.h>
#include <cvt/gfx/IScaleFilter.h>

namespace cvt
{

#define Compare(X, Y) ((X)>=(Y))

	FAST::FAST( FASTSize size ) :
		_threshold( 30 ),
        _suppress( true ),
        _fastSize( size )
	{
	}

	FAST::~FAST()
	{
	}

    void FAST::nonmaxSuppression( std::vector<Feature2Df> & suppressed, const std::vector<Feature2Df> & features )
    {
        int numCorners = (int)features.size();
        int endRow = (int)features.back().pt.y;        
        
		int firstFeatureIdxInRow[ endRow + 1 ];
        memset( firstFeatureIdxInRow, -1, (endRow + 1) * sizeof(int) );
        
        // initialize the indizes
		{
			int prev_row = -1;
            for( int i = 0; i < numCorners; i++ ){
                int currRow = ( int )features[ i ].pt.y;
                if(  currRow != prev_row ) {
                    firstFeatureIdxInRow[ currRow ] = i;
					prev_row = currRow;
				}
            }
		}
        
        int idx, row, col;
        for( int i = 0; i < numCorners; i++ ) {
			float score = features[ i ].score;
            col = ( int )features[ i ].pt.x;
            row = ( int )features[ i ].pt.y;
            
            /* Check left */
			if( i > 0 ){
                idx = i-1;
				if( ( int )features[ idx ].pt.x == ( col - 1 ) && (int)features[ idx ].pt.y == row ){
                    if( score <= features[ idx ].score  ){
                        // left has better or same score:
                        continue;
                    }
                }
            }
            
            if( i < numCorners-1 ){
                idx = i+1;
				if( ( int )features[ idx ].pt.x == ( col + 1 ) && (int)features[ idx ].pt.y == row ){
                    if( score < features[ idx ].score  ){
                        // right has better score:
                        continue;
                    }
                }
            }
            
            bool thereIsABetterPoint = false;
			/* Check above (if there is a valid row above) */
            int pointIdxAbove;
			if( row > 0 && ( pointIdxAbove = firstFeatureIdxInRow[ row - 1 ] ) != -1 ){
				/* Make sure that current point_above is one row above. */
				while( (int)features[ pointIdxAbove ].pt.y == row - 1 &&
                      (int)features[ pointIdxAbove ].pt.x < col - 1 )
                    pointIdxAbove++;
                
                while( (int)features[ pointIdxAbove ].pt.y == row - 1 &&
                      (int)features[ pointIdxAbove ].pt.x < col + 1 ){
                    // check the three pixels above
                    if( score <= features[ pointIdxAbove ].score ){
                        thereIsABetterPoint = true;
                        break;
                    }
                    pointIdxAbove++;
                }
                
                if( thereIsABetterPoint )
                    continue;
			}
            
            if( row < endRow ){
				/* Make sure that current point_above is one row above. */
                int pointIdx = firstFeatureIdxInRow[ row + 1 ];
                
                if( pointIdx == -1 )
                    continue;
                
				while( pointIdx < numCorners && (int)features[ pointIdx ].pt.x < col - 1 )
                    pointIdx++;
                
                while( pointIdx < numCorners && (int)features[ pointIdx ].pt.x < col + 1 ){
                    // check the three pixels above
                    if( score < features[ pointIdx ].score ){
                        thereIsABetterPoint = true;
                        break;
                    }
                    pointIdx++;
                }
                
                if( thereIsABetterPoint )
                    continue;
			}
            
			suppressed.push_back( features[ i ] );
		}
    }


}
