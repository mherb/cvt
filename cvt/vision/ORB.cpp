#include <cvt/vision/ORB.h>

#include <algorithm>

namespace cvt {

	ORB::ORB( const Image& img, size_t octaves, float scalefactor, uint8_t cornerThreshold ) :
        _threshold( cornerThreshold )
	{
		float scale = 1.0f;
		IScaleFilterBilinear scaleFilter;

        _scaleFactors = new float[ octaves ];
        _iimages = new IntegralImage[ octaves ];

        _features.reserve( 512 );
        _scaleFactors[ 0 ] = scale;

        std::vector<ORBFeature> octaveFeatures;
        octaveFeatures.reserve( 512 );
		detect( octaveFeatures, img, 0 );

        nonmaxSuppression( octaveFeatures );

		for( size_t i = 1; i < octaves; i++ ) {
            octaveFeatures.clear();
            octaveFeatures.reserve( 512 );

			Image pyrimg;
			scale *=  scalefactor;
			img.scale( pyrimg, ( size_t )( img.width() * scale ), ( size_t )( img.height() * scale ), scaleFilter );
            _scaleFactors[ i ] = scale;
			detect( octaveFeatures, pyrimg, i );
            nonmaxSuppression( octaveFeatures );
		}
        selectBestFeatures( 2000 );

        extract( octaves );
	}

    ORB::~ORB()
    {
        delete[] _scaleFactors;
        delete[] _iimages;
    }

	void ORB::detect( std::vector<ORBFeature> & features, const Image& img, size_t octave )
	{
        // detect the features for this level
        size_t stride;
        const uint8_t * ptr = img.map( &stride );
        //detect9( features, ptr, stride, img.width(), img.height(), octave );
        detect9simd( features, ptr, stride, img.width(), img.height(), octave );

        SIMD * simd = SIMD::instance();

        for( size_t i = 0; i < features.size(); i++ ){
            features[ i ].score = simd->harrisResponse1u8( ptr + (int)features[ i ].pt.y * stride + (int)features[ i ].pt.x , stride, 4, 4, 0.04f );
        }

        img.unmap( ptr );

        _iimages[ octave ].update( img );
	}

    void ORB::nonmaxSuppression( const std::vector<ORBFeature> & features )
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

			_features.push_back( features[ i ] );
		}
    }

    void ORB::extract( size_t octaves )
    {
        const float* iimgptr[ octaves ];
        size_t strides[ octaves ];

        for( size_t i = 0; i < octaves; i++ ){
            iimgptr[ i ] = _iimages[ i ].sumImage().map<float>( &strides[ i ] );
        }

        for( size_t i = 0, iend = _features.size(); i < iend; i++ ){
            size_t octave = _features[ i ].octave;
            centroidAngle( _features[ i ], iimgptr[ octave ], strides[ octave ] );
            descriptor( _features[ i ], iimgptr[ octave ], strides[ octave ] );
        }


        for( size_t i = 0; i < octaves; i++ ){
            _iimages[ i ].sumImage().unmap<float>( iimgptr[ i ] );
        }
    }

	void ORB::centroidAngle( ORBFeature& feature, const float* iimgptr, size_t widthstep )
	{
		float mx = 0;
		float my = 0;

		int cury = ( int ) feature.pt.y - 15;
		int curx = ( int ) feature.pt.x;

		for( int i = 0; i < 15; i++ ) {
			mx +=( ( float ) i - 15.0f ) * ( IntegralImage::area( iimgptr, curx - _circularoffset[ i ], cury + i, 2 * _circularoffset[ i ] + 1, 1, widthstep )
										   - IntegralImage::area( iimgptr, curx - _circularoffset[ i ], cury + 30 - i, 2 * _circularoffset[ i ] + 1, 1, widthstep ) );
		}

		cury = ( int ) feature.pt.y;
		curx = ( int ) feature.pt.x - 15;
		for( int i = 0; i < 15; i++ ) {
			my += ( ( float ) i - 15.0f ) * ( IntegralImage::area( iimgptr, curx + i, cury - _circularoffset[ i ], 1, 2 * _circularoffset[ i ] + 1, widthstep )
										    - IntegralImage::area( iimgptr, curx + 30 - i, cury - _circularoffset[ i ], 1, 2 * _circularoffset[ i ] + 1, widthstep ) );
		}

		feature.angle = Math::atan2( my, mx );


        if( feature.angle < 0 )
			feature.angle += Math::TWO_PI;
		feature.angle = Math::TWO_PI - feature.angle;
	}

	void ORB::descriptor( ORBFeature& feature, const float* iimgptr, size_t widthstep )
	{
		size_t index = ( size_t ) ( feature.angle * 30.0f / Math::TWO_PI );
		if( index >= 30 )
			index = 0;
		int x = ( int ) feature.pt.x;
		int y = ( int ) feature.pt.y;


#define ORBTEST( n ) ( IntegralImage::area( iimgptr, x + _patterns[ index ][ ( n ) * 2 ][ 0 ] - 2, y + _patterns[ index ][ ( n ) * 2 ][ 1 ] -2, 5, 5, widthstep ) < \
					   IntegralImage::area( iimgptr, x + _patterns[ index ][ ( n ) * 2 + 1 ][ 0 ] - 2, y + _patterns[ index ][ ( n ) * 2 + 1 ][ 1 ] -2, 5, 5, widthstep ) )

		for( int i = 0; i < 32; i++ ) {
			feature.desc[ i ] = 0;
			for( int k = 0; k < 8; k++ ) {
				feature.desc[ i ] |= ( ORBTEST( i * 8 + k ) ) << k;
			}
		}
		feature.pt /= _scaleFactors[ feature.octave ];
	}

	static bool compareOrbFeature( const ORBFeature & a, const ORBFeature & b )
	{
		return a.score > b.score;
	}

	void ORB::selectBestFeatures( size_t num )
	{
		std::sort( _features.begin(), _features.end(), compareOrbFeature );

		if( _features.size() > num )
			_features.erase( _features.begin() + num, _features.end() );
	}

    void ORB::detect9( std::vector<ORBFeature> & features, const uint8_t* im, size_t stride, size_t width, size_t height, size_t octave )
    {
		makeOffsets( stride );
        size_t h = height - _border;
        size_t w = width - _border;

        im += ( _border * stride + _border );

        int upperBound;
        int lowerBound;

		SIMD * simd = SIMD::instance();

		static const float harrisK = 0.04f;
		static const float harrisThreshold = 1e5f;
		static const size_t harrisRadius = 4;

        for( size_t y = _border; y < h; y++ ){
            const uint8_t * curr = im;

            for( size_t x = _border; x < w; x++ ){
                lowerBound = *curr - _threshold;
                upperBound = *curr + _threshold;

                if( lowerBound > 0 && isDarkerCorner9( curr, lowerBound ) ) {
					float harris = simd->harrisResponse1u8( curr, stride, harrisRadius, harrisRadius, harrisK /* k from Pollefeys slides */ );
					if( harris > harrisThreshold )
						features.push_back( ORBFeature( x, y, 0.0f, octave, harris ) );
                } else if( upperBound < 255 && isBrighterCorner9( curr, upperBound ) ) {
					float harris = simd->harrisResponse1u8( curr, stride, harrisRadius, harrisRadius, harrisK );
					if( harris > harrisThreshold )
                        features.push_back( ORBFeature( x, y, 0.0f, octave, harris ) );
                }
                curr++;
            }
            im += stride;
        }
    }

    void ORB::makeOffsets( size_t row_stride )
	{
        _pixel[0]  =  0 + row_stride * 3;
		_pixel[1]  =  1 + row_stride * 3;
		_pixel[2]  =  2 + row_stride * 2;
		_pixel[3]  =  3 + row_stride * 1;
		_pixel[4]  =  3;
		_pixel[5]  =  3 - row_stride * 1;
		_pixel[6]  =  2 - row_stride * 2;
		_pixel[7]  =  1 - row_stride * 3;
		_pixel[8]  =    - row_stride * 3;
		_pixel[9]  = -1 - row_stride * 3;
		_pixel[10] = -2 - row_stride * 2;
		_pixel[11] = -3 - row_stride * 1;
		_pixel[12] = -3;
		_pixel[13] = -3 + row_stride;
		_pixel[14] = -2 + row_stride * 2;
		_pixel[15] = -1 + row_stride * 3;
    }

    bool ORB::isDarkerCorner9( const uint8_t * p, const int barrier )
	{
		if( !( p[ _pixel[ 15 ] ] < barrier ) ) {     // ???????????????-
            if( !(p[ _pixel[ 8 ] ] < barrier ) ) {   // ????????-??????-
				return false;
			}                                       // ????????@??????-
            if( !(p[ _pixel[ 7 ] ] < barrier ) ) {   // ???????-@??????-
				return false;
			}										// ???????@@??????-
            if( !(p[ _pixel[ 6 ] ] < barrier ) ) {   // ??????-@@??????-
				return false;
			}                                           // ??????@@@??????-
            if( !(p[ _pixel[ 3 ] ] < barrier ) ) {		// ??????@@@??????-
                if( !(p[ _pixel[ 10 ] ] < barrier ) ){  // ?????-@@@?????--
					return false;
				}                               // ?????-@@@?????@- :p13
                if( !(p[ _pixel[ 11 ] ] < barrier ) ) {   // ?????-@@@????-@-
					return false;
				}                               // ?????-@@@????@@-
                if( !(p[ _pixel[ 15 ] ] < barrier ) ) {  // ?????-@@@-???@@-
					return false;
				}                               // ?????-@@@@???@@-
                if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // ?????-@@@@-??@@-
					return false;
				} // ?????-@@@@@??@@-
                if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // ?????-@@@@@-?@@-
					return false;
				} // ?????-@@@@@@?@@-
                if( !(p[ _pixel[ 12 ] ] < barrier ) ) { // ?????-@@@@@@-@@-
					return false;
				} // ?????-@@@@@@@@@-
				return true;
			} // ?????@@@@??????-
            if( !(p[ _pixel[ 4 ] ] < barrier ) ) { // ????-@@@@??????-
                if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // ????-@@@@????-?-
					return false;
				} // ????-@@@@????@?-
                if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // ????-@@@@-???@?-
					return false;
				} // ????-@@@@@???@?-
                if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // ????-@@@@@-??@?-
					return false;
				} // ????-@@@@@@??@?-
                if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // ????-@@@@@@-?@?-
					return false;
				} // ????-@@@@@@@?@?-
                if( !(p[ _pixel[ 12 ] ] < barrier ) ) { // ????-@@@@@@@-@?-
					return false;
				} // ????-@@@@@@@@@?-
				return true;
			} // ????@@@@@??????-
            if( !(p[ _pixel[ 5 ] ] < barrier ) ) { // ???-@@@@@??????-
                if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // ???-@@@@@-?????-
					return false;
				} // ???-@@@@@@?????-
                if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // ???-@@@@@@-????-
					return false;
				} // ???-@@@@@@@????-
                if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // ???-@@@@@@@-???-
					return false;
				} // ???-@@@@@@@@???-
                if( !(p[ _pixel[ 12 ] ] < barrier ) ) { // ???-@@@@@@@@-??-
					return false;
				} // ???-@@@@@@@@@??-
				return true;
			} // ???@@@@@@??????-
            if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // ???@@@@@@-?????-
                if( !(p[ _pixel[ 8 ] ] < barrier ) ) { // -??@@@@@@-?????-
					return false;
				} // @??@@@@@@-?????-
                if( !(p[ _pixel[ 7 ] ] < barrier ) ) { // @-?@@@@@@-?????-
					return false;
				} // @@?@@@@@@-?????-
                if( !(p[ _pixel[ 6 ] ] < barrier ) ) { // @@-@@@@@@-?????-
					return false;
				} // @@@@@@@@@-?????-
				return true;
			} // ???@@@@@@@?????-
            if( !(p[ _pixel[ 6 ] ] < barrier ) ) { // ??-@@@@@@@?????-
                if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // ??-@@@@@@@-????-
					return false;
				} // ??-@@@@@@@@????-
                if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // ??-@@@@@@@@-???-
					return false;
				} // ??-@@@@@@@@@???-
				return true;
			} // ??@@@@@@@@?????-
            if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // ??@@@@@@@@-????-
                if( !(p[ _pixel[ 7 ] ] < barrier ) ) { // ?-@@@@@@@@-????-
					return false;
				} // ?@@@@@@@@@-????-
				return true;
			} // ??@@@@@@@@@????-
			return true;
		} // ???????????????@
        if( !(p[ _pixel[ 10 ] ] < barrier ) ) { // ??????????????-@
            if( !(p[ _pixel[ 3 ] ] < barrier ) ) { // ?????-????????-@
				return false;
			} // ?????@????????-@
            if( !(p[ _pixel[ 1 ]] < barrier ) ) { // ?????@?-??????-@
				return false;
			} // ?????@?@??????-@
            if( !(p[ _pixel[ 2 ] ] < barrier ) ) { // ?????@-@??????-@
				return false;
			} // ?????@@@??????-@
            if( !(p[ _pixel[ 4 ] ] < barrier ) ) { // ????-@@@??????-@
                if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // ????-@@@?????--@
					return false;
				} // ????-@@@?????@-@
                if( !(p[ _pixel[ 12 ] ] < barrier ) ) { // ????-@@@????-@-@
					return false;
				} // ????-@@@????@@-@
                if( !(p[ _pixel[ 0 ]] < barrier ) ) { // ????-@@@-???@@-@
					return false;
				} // ????-@@@@???@@-@
                if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // ????-@@@@-??@@-@
					return false;
				} // ????-@@@@@??@@-@
                if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // ????-@@@@@-?@@-@
					return false;
				} // ????-@@@@@@?@@-@
                if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // ????-@@@@@@-@@-@
					return false;
				} // ????-@@@@@@@@@-@
				return true;
			} // ????@@@@??????-@
            if( !(p[ _pixel[ 5 ] ] < barrier ) ) { // ???-@@@@??????-@
                if( !(p[ _pixel[ 12 ] ] < barrier ) ) { // ???-@@@@????-?-@
					return false;
				} // ???-@@@@????@?-@
                if( !(p[ _pixel[ 0 ]] < barrier ) ) { // ???-@@@@-???@?-@
					return false;
				} // ???-@@@@@???@?-@
                if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // ???-@@@@@-??@?-@
					return false;
				} // ???-@@@@@@??@?-@
                if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // ???-@@@@@@-?@?-@
					return false;
				} // ???-@@@@@@@?@?-@
                if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // ???-@@@@@@@-@?-@
					return false;
				} // ???-@@@@@@@@@?-@
				return true;
			} // ???@@@@@??????-@
            if( !(p[ _pixel[ 6 ] ] < barrier ) ) { // ??-@@@@@??????-@
                if( !(p[ _pixel[ 0 ]] < barrier ) ) { // ??-@@@@@-?????-@
					return false;
				} // ??-@@@@@@?????-@
                if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // ??-@@@@@@-????-@
					return false;
				} // ??-@@@@@@@????-@
                if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // ??-@@@@@@@-???-@
					return false;
				} // ??-@@@@@@@@???-@
                if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // ??-@@@@@@@@-??-@
					return false;
				} // ??-@@@@@@@@@??-@
				return true;
			} // ??@@@@@@??????-@
            if( !(p[ _pixel[ 7 ] ] < barrier ) ) { // ?-@@@@@@??????-@
                if( !(p[ _pixel[ 0 ]] < barrier ) ) { // ?-@@@@@@-?????-@
					return false;
				} // ?-@@@@@@@?????-@
                if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // ?-@@@@@@@-????-@
					return false;
				} // ?-@@@@@@@@????-@
                if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // ?-@@@@@@@@-???-@
					return false;
				} // ?-@@@@@@@@@???-@
				return true;
			} // ?@@@@@@@??????-@
            if( !(p[ _pixel[ 8 ] ] < barrier ) ) { // -@@@@@@@??????-@
                if( !(p[ _pixel[ 0 ]] < barrier ) ) { // -@@@@@@@-?????-@
					return false;
				} // -@@@@@@@@?????-@
                if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // -@@@@@@@@-????-@
					return false;
				} // -@@@@@@@@@????-@
				return true;
			} // @@@@@@@@??????-@
			return true;
		} // ??????????????@@
        if( !(p[ _pixel[ 8 ] ] < barrier ) ) { // -?????????????@@
            if( !(p[ _pixel[ 1 ]] < barrier ) ) { // -??????-??????@@
				return false;
			} // -??????@??????@@
            if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // -??????@?-????@@
				return false;
			} // -??????@?@????@@
            if( !(p[ _pixel[ 0 ]] < barrier ) ) { // -??????@-@????@@
				return false;
			} // -??????@@@????@@
            if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // -??????@@@-???@@
                if( !(p[ _pixel[ 2 ] ] < barrier ) ) { // -?????-@@@-???@@
					return false;
				} // -?????@@@@-???@@
                if( !(p[ _pixel[ 3 ] ] < barrier ) ) { // -????-@@@@-???@@
					return false;
				} // -????@@@@@-???@@
                if( !(p[ _pixel[ 7 ] ] < barrier ) ) { // --???@@@@@-???@@
					return false;
				} // -@???@@@@@-???@@
                if( !(p[ _pixel[ 6 ] ] < barrier ) ) { // -@-??@@@@@-???@@
					return false;
				} // -@@??@@@@@-???@@
                if( !(p[ _pixel[ 5 ] ] < barrier ) ) { // -@@-?@@@@@-???@@
					return false;
				} // -@@@?@@@@@-???@@
                if( !(p[ _pixel[ 4 ] ] < barrier ) ) { // -@@@-@@@@@-???@@
					return false;
				} // -@@@@@@@@@-???@@
				return true;
			} // -??????@@@@???@@
            if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // -??????@@@@-??@@
                if( !(p[ _pixel[ 2 ] ] < barrier ) ) { // -?????-@@@@-??@@
					return false;
				} // -?????@@@@@-??@@
                if( !(p[ _pixel[ 6 ] ] < barrier ) ) { // -?-???@@@@@-??@@
					return false;
				} // -?@???@@@@@-??@@
                if( !(p[ _pixel[ 5 ] ] < barrier ) ) { // -?@-??@@@@@-??@@
					return false;
				} // -?@@??@@@@@-??@@
                if( !(p[ _pixel[ 4 ] ] < barrier ) ) { // -?@@-?@@@@@-??@@
					return false;
				} // -?@@@?@@@@@-??@@
                if( !(p[ _pixel[ 3 ] ] < barrier ) ) { // -?@@@-@@@@@-??@@
					return false;
				} // -?@@@@@@@@@-??@@
				return true;
			} // -??????@@@@@??@@
            if( !(p[ _pixel[ 12 ] ] < barrier ) ) { // -??????@@@@@-?@@
                if( !(p[ _pixel[ 5 ] ] < barrier ) ) { // -??-???@@@@@-?@@
					return false;
				} // -??@???@@@@@-?@@
                if( !(p[ _pixel[ 4 ] ] < barrier ) ) { // -??@-??@@@@@-?@@
					return false;
				} // -??@@??@@@@@-?@@
                if( !(p[ _pixel[ 3 ] ] < barrier ) ) { // -??@@-?@@@@@-?@@
					return false;
				} // -??@@@?@@@@@-?@@
                if( !(p[ _pixel[ 2 ] ] < barrier ) ) { // -??@@@-@@@@@-?@@
					return false;
				} // -??@@@@@@@@@-?@@
				return true;
			} // -??????@@@@@@?@@
            if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // -??????@@@@@@-@@
                if( !(p[ _pixel[ 4 ] ] < barrier ) ) { // -???-??@@@@@@-@@
					return false;
				} // -???@??@@@@@@-@@
                if( !(p[ _pixel[ 3 ] ] < barrier ) ) { // -???@-?@@@@@@-@@
					return false;
				} // -???@@?@@@@@@-@@
                if( !(p[ _pixel[ 2 ] ] < barrier ) ) { // -???@@-@@@@@@-@@
					return false;
				} // -???@@@@@@@@@-@@
				return true;
			} // -??????@@@@@@@@@
			return true;
		} // @?????????????@@
        if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // @????????????-@@
            if( !(p[ _pixel[ 2 ] ] < barrier ) ) { // @?????-??????-@@
				return false;
			} // @?????@??????-@@
            if( !(p[ _pixel[ 3 ] ] < barrier ) ) { // @????-@??????-@@
				return false;
			} // @????@@??????-@@
            if( !(p[ _pixel[ 4 ] ] < barrier ) ) { // @???-@@??????-@@
				return false;
			} // @???@@@??????-@@
            if( !(p[ _pixel[ 5 ] ] < barrier ) ) { // @??-@@@??????-@@
                if( !(p[ _pixel[ 12 ] ] < barrier ) ) { // @??-@@@?????--@@
					return false;
				} // @??-@@@?????@-@@
                if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // @??-@@@????-@-@@
					return false;
				} // @??-@@@????@@-@@
                if( !(p[ _pixel[ 1 ]] < barrier ) ) { // @??-@@@-???@@-@@
					return false;
				} // @??-@@@@???@@-@@
                if( !(p[ _pixel[ 0 ]] < barrier ) ) { // @??-@@@@-??@@-@@
					return false;
				} // @??-@@@@@??@@-@@
                if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // @??-@@@@@-?@@-@@
					return false;
				} // @??-@@@@@@?@@-@@
                if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // @??-@@@@@@-@@-@@
					return false;
				} // @??-@@@@@@@@@-@@
				return true;
			} // @??@@@@??????-@@
            if( !(p[ _pixel[ 6 ] ] < barrier ) ) { // @?-@@@@??????-@@
                if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // @?-@@@@????-?-@@
					return false;
				} // @?-@@@@????@?-@@
                if( !(p[ _pixel[ 1 ]] < barrier ) ) { // @?-@@@@-???@?-@@
					return false;
				} // @?-@@@@@???@?-@@
                if( !(p[ _pixel[ 0 ]] < barrier ) ) { // @?-@@@@@-??@?-@@
					return false;
				} // @?-@@@@@@??@?-@@
                if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // @?-@@@@@@-?@?-@@
					return false;
				} // @?-@@@@@@@?@?-@@
                if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // @?-@@@@@@@-@?-@@
					return false;
				} // @?-@@@@@@@@@?-@@
				return true;
			} // @?@@@@@??????-@@
            if( !(p[ _pixel[ 7 ] ] < barrier ) ) { // @-@@@@@??????-@@
                if( !(p[ _pixel[ 1 ]] < barrier ) ) { // @-@@@@@-?????-@@
					return false;
				} // @-@@@@@@?????-@@
                if( !(p[ _pixel[ 0 ]] < barrier ) ) { // @-@@@@@@-????-@@
					return false;
				} // @-@@@@@@@????-@@
                if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // @-@@@@@@@-???-@@
					return false;
				} // @-@@@@@@@@???-@@
                if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // @-@@@@@@@@-??-@@
					return false;
				} // @-@@@@@@@@@??-@@
				return true;
			} // @@@@@@@??????-@@
			return true;
		} // @????????????@@@
        if( !(p[ _pixel[ 7 ] ] < barrier ) ) { // @-???????????@@@
            if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // @-????????-??@@@
				return false;
			} // @-????????@??@@@
            if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // @-???????-@??@@@
				return false;
			} // @-???????@@??@@@
            if( !(p[ _pixel[ 0 ]] < barrier ) ) { // @-??????-@@??@@@
				return false;
			} // @-??????@@@??@@@
            if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // @-??????@@@-?@@@
                if( !(p[ _pixel[ 1 ]] < barrier ) ) { // @-?????-@@@-?@@@
					return false;
				} // @-?????@@@@-?@@@
                if( !(p[ _pixel[ 2 ] ] < barrier ) ) { // @-????-@@@@-?@@@
					return false;
				} // @-????@@@@@-?@@@
                if( !(p[ _pixel[ 6 ] ] < barrier ) ) { // @--???@@@@@-?@@@
					return false;
				} // @-@???@@@@@-?@@@
                if( !(p[ _pixel[ 5 ] ] < barrier ) ) { // @-@-??@@@@@-?@@@
					return false;
				} // @-@@??@@@@@-?@@@
                if( !(p[ _pixel[ 4 ] ] < barrier ) ) { // @-@@-?@@@@@-?@@@
					return false;
				} // @-@@@?@@@@@-?@@@
                if( !(p[ _pixel[ 3 ] ] < barrier ) ) { // @-@@@-@@@@@-?@@@
					return false;
				} // @-@@@@@@@@@-?@@@
				return true;
			} // @-??????@@@@?@@@
            if( !(p[ _pixel[ 12 ] ] < barrier ) ) { // @-??????@@@@-@@@
                if( !(p[ _pixel[ 1 ]] < barrier ) ) { // @-?????-@@@@-@@@
					return false;
				} // @-?????@@@@@-@@@
                if( !(p[ _pixel[ 5 ] ] < barrier ) ) { // @-?-???@@@@@-@@@
					return false;
				} // @-?@???@@@@@-@@@
                if( !(p[ _pixel[ 4 ] ] < barrier ) ) { // @-?@-??@@@@@-@@@
					return false;
				} // @-?@@??@@@@@-@@@
                if( !(p[ _pixel[ 3 ] ] < barrier ) ) { // @-?@@-?@@@@@-@@@
					return false;
				} // @-?@@@?@@@@@-@@@
                if( !(p[ _pixel[ 2 ] ] < barrier ) ) { // @-?@@@-@@@@@-@@@
					return false;
				} // @-?@@@@@@@@@-@@@
				return true;
			} // @-??????@@@@@@@@
			return true;
		} // @@???????????@@@
        if( !(p[ _pixel[ 12 ] ] < barrier ) ) { // @@??????????-@@@
            if( !(p[ _pixel[ 3 ] ] < barrier ) ) { // @@???-??????-@@@
				return false;
			} // @@???@??????-@@@
            if( !(p[ _pixel[ 4 ] ] < barrier ) ) { // @@??-@??????-@@@
				return false;
			} // @@??@@??????-@@@
            if( !(p[ _pixel[ 5 ] ] < barrier ) ) { // @@?-@@??????-@@@
				return false;
			} // @@?@@@??????-@@@
            if( !(p[ _pixel[ 6 ] ] < barrier ) ) { // @@-@@@??????-@@@
                if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // @@-@@@?????--@@@
					return false;
				} // @@-@@@?????@-@@@
                if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // @@-@@@????-@-@@@
					return false;
				} // @@-@@@????@@-@@@
                if( !(p[ _pixel[ 2 ] ] < barrier ) ) { // @@-@@@-???@@-@@@
					return false;
				} // @@-@@@@???@@-@@@
                if( !(p[ _pixel[ 1 ]] < barrier ) ) { // @@-@@@@-??@@-@@@
					return false;
				} // @@-@@@@@??@@-@@@
                if( !(p[ _pixel[ 0 ]] < barrier ) ) { // @@-@@@@@-?@@-@@@
					return false;
				} // @@-@@@@@@?@@-@@@
                if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // @@-@@@@@@-@@-@@@
					return false;
				} // @@-@@@@@@@@@-@@@
				return true;
			} // @@@@@@??????-@@@
			return true;
		} // @@??????????@@@@
        if( !(p[ _pixel[ 6 ] ] < barrier ) ) { // @@-?????????@@@@
            if( !(p[ _pixel[ 15 ] ] < barrier ) ) { // @@-??????-??@@@@
				return false;
			} // @@-??????@??@@@@
            if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // @@-??????@-?@@@@
				return false;
			} // @@-??????@@?@@@@
            if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // @@-??????@@-@@@@
				return false;
			} // @@-??????@@@@@@@
			return true;
		} // @@@?????????@@@@
        if( !(p[ _pixel[ 11 ] ] < barrier ) ) { // @@@????????-@@@@
            if( !(p[ _pixel[ 5 ] ] < barrier ) ) { // @@@-???????-@@@@
				return false;
			} // @@@@???????-@@@@
            if( !(p[ _pixel[ 4 ] ] < barrier ) ) { // @@@@-??????-@@@@
				return false;
			} // @@@@@??????-@@@@
			return true;
		} // @@@????????@@@@@
        if( !(p[ _pixel[ 5 ] ] < barrier ) ) { // @@@-???????@@@@@
            if( !(p[ _pixel[ 14 ] ] < barrier ) ) { // @@@-??????-@@@@@
				return false;
			} // @@@-??????@@@@@@
			return true;
		} // @@@@???????@@@@@
		return true;
	}
    /* pixel, stride, 3x stride barrier (center+thres)
	 */
    bool ORB::isBrighterCorner9( const uint8_t * p, const int barrier )
	{
		if( !( p[ _pixel[ 9 ] ] > barrier ) ) {	// ???????????????-
            if( !(p[ _pixel[ 0 ]] > barrier ) ) {		// ???????-???????-
				return false;
			}								// ????????@??????-
            if( !(p[ _pixel[ 1 ]] > barrier ) ) {		// ???????-@??????-
				return false;
			}								// ???????@@??????-
            if( !(p[ _pixel[ 2 ] ] > barrier ) ) {		// ??????-@@??????-
				return false;
			}								// ??????@@@??????-
            if( !(p[ _pixel[ 3 ] ] > barrier ) ) {		// ?????-@@@??????-
                if( !(p[ _pixel[ 10 ] ] > barrier ) ){ // ?????-@@@?????--
					return false;
				} // ?????-@@@?????@-
                if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // ?????-@@@????-@-
					return false;
				} // ?????-@@@????@@-
                if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // ?????-@@@-???@@-
					return false;
				} // ?????-@@@@???@@-
                if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // ?????-@@@@-??@@-
					return false;
				} // ?????-@@@@@??@@-
                if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // ?????-@@@@@-?@@-
					return false;
				} // ?????-@@@@@@?@@-
                if( !(p[ _pixel[ 12 ] ] > barrier ) ) { // ?????-@@@@@@-@@-
					return false;
				} // ?????-@@@@@@@@@-
				return true;
			} // ?????@@@@??????-
            if( !(p[ _pixel[ 4 ] ] > barrier ) ) { // ????-@@@@??????-
                if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // ????-@@@@????-?-
					return false;
				} // ????-@@@@????@?-
                if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // ????-@@@@-???@?-
					return false;
				} // ????-@@@@@???@?-
                if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // ????-@@@@@-??@?-
					return false;
				} // ????-@@@@@@??@?-
                if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // ????-@@@@@@-?@?-
					return false;
				} // ????-@@@@@@@?@?-
                if( !(p[ _pixel[ 12 ] ] > barrier ) ) { // ????-@@@@@@@-@?-
					return false;
				} // ????-@@@@@@@@@?-
				return true;
			} // ????@@@@@??????-
            if( !(p[ _pixel[ 5 ] ] > barrier ) ) { // ???-@@@@@??????-
                if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // ???-@@@@@-?????-
					return false;
				} // ???-@@@@@@?????-
                if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // ???-@@@@@@-????-
					return false;
				} // ???-@@@@@@@????-
                if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // ???-@@@@@@@-???-
					return false;
				} // ???-@@@@@@@@???-
                if( !(p[ _pixel[ 12 ] ] > barrier ) ) { // ???-@@@@@@@@-??-
					return false;
				} // ???-@@@@@@@@@??-
				return true;
			} // ???@@@@@@??????-
            if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // ???@@@@@@-?????-
                if( !(p[ _pixel[ 8 ] ] > barrier ) ) { // -??@@@@@@-?????-
					return false;
				} // @??@@@@@@-?????-
                if( !(p[ _pixel[ 7 ] ] > barrier ) ) { // @-?@@@@@@-?????-
					return false;
				} // @@?@@@@@@-?????-
                if( !(p[ _pixel[ 6 ] ] > barrier ) ) { // @@-@@@@@@-?????-
					return false;
				} // @@@@@@@@@-?????-
				return true;
			} // ???@@@@@@@?????-
            if( !(p[ _pixel[ 6 ] ] > barrier ) ) { // ??-@@@@@@@?????-
                if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // ??-@@@@@@@-????-
					return false;
				} // ??-@@@@@@@@????-
                if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // ??-@@@@@@@@-???-
					return false;
				} // ??-@@@@@@@@@???-
				return true;
			} // ??@@@@@@@@?????-
            if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // ??@@@@@@@@-????-
                if( !(p[ _pixel[ 7 ] ] > barrier ) ) { // ?-@@@@@@@@-????-
					return false;
				} // ?@@@@@@@@@-????-
				return true;
			} // ??@@@@@@@@@????-
			return true;
		} // ???????????????@
        if( !(p[ _pixel[ 10 ] ] > barrier ) ) { // ??????????????-@
            if( !(p[ _pixel[ 3 ] ] > barrier ) ) { // ?????-????????-@
				return false;
			} // ?????@????????-@
            if( !(p[ _pixel[ 1 ]] > barrier ) ) { // ?????@?-??????-@
				return false;
			} // ?????@?@??????-@
            if( !(p[ _pixel[ 2 ] ] > barrier ) ) { // ?????@-@??????-@
				return false;
			} // ?????@@@??????-@
            if( !(p[ _pixel[ 4 ] ] > barrier ) ) { // ????-@@@??????-@
                if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // ????-@@@?????--@
					return false;
				} // ????-@@@?????@-@
                if( !(p[ _pixel[ 12 ] ] > barrier ) ) { // ????-@@@????-@-@
					return false;
				} // ????-@@@????@@-@
                if( !(p[ _pixel[ 0 ]] > barrier ) ) { // ????-@@@-???@@-@
					return false;
				} // ????-@@@@???@@-@
                if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // ????-@@@@-??@@-@
					return false;
				} // ????-@@@@@??@@-@
                if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // ????-@@@@@-?@@-@
					return false;
				} // ????-@@@@@@?@@-@
                if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // ????-@@@@@@-@@-@
					return false;
				} // ????-@@@@@@@@@-@
				return true;
			} // ????@@@@??????-@
            if( !(p[ _pixel[ 5 ] ] > barrier ) ) { // ???-@@@@??????-@
                if( !(p[ _pixel[ 12 ] ] > barrier ) ) { // ???-@@@@????-?-@
					return false;
				} // ???-@@@@????@?-@
                if( !(p[ _pixel[ 0 ]] > barrier ) ) { // ???-@@@@-???@?-@
					return false;
				} // ???-@@@@@???@?-@
                if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // ???-@@@@@-??@?-@
					return false;
				} // ???-@@@@@@??@?-@
                if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // ???-@@@@@@-?@?-@
					return false;
				} // ???-@@@@@@@?@?-@
                if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // ???-@@@@@@@-@?-@
					return false;
				} // ???-@@@@@@@@@?-@
				return true;
			} // ???@@@@@??????-@
            if( !(p[ _pixel[ 6 ] ] > barrier ) ) { // ??-@@@@@??????-@
                if( !(p[ _pixel[ 0 ]] > barrier ) ) { // ??-@@@@@-?????-@
					return false;
				} // ??-@@@@@@?????-@
                if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // ??-@@@@@@-????-@
					return false;
				} // ??-@@@@@@@????-@
                if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // ??-@@@@@@@-???-@
					return false;
				} // ??-@@@@@@@@???-@
                if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // ??-@@@@@@@@-??-@
					return false;
				} // ??-@@@@@@@@@??-@
				return true;
			} // ??@@@@@@??????-@
            if( !(p[ _pixel[ 7 ] ] > barrier ) ) { // ?-@@@@@@??????-@
                if( !(p[ _pixel[ 0 ]] > barrier ) ) { // ?-@@@@@@-?????-@
					return false;
				} // ?-@@@@@@@?????-@
                if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // ?-@@@@@@@-????-@
					return false;
				} // ?-@@@@@@@@????-@
                if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // ?-@@@@@@@@-???-@
					return false;
				} // ?-@@@@@@@@@???-@
				return true;
			} // ?@@@@@@@??????-@
            if( !(p[ _pixel[ 8 ] ] > barrier ) ) { // -@@@@@@@??????-@
                if( !(p[ _pixel[ 0 ]] > barrier ) ) { // -@@@@@@@-?????-@
					return false;
				} // -@@@@@@@@?????-@
                if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // -@@@@@@@@-????-@
					return false;
				} // -@@@@@@@@@????-@
				return true;
			} // @@@@@@@@??????-@
			return true;
		} // ??????????????@@
        if( !(p[ _pixel[ 8 ] ] > barrier ) ) { // -?????????????@@
            if( !(p[ _pixel[ 1 ]] > barrier ) ) { // -??????-??????@@
				return false;
			} // -??????@??????@@
            if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // -??????@?-????@@
				return false;
			} // -??????@?@????@@
            if( !(p[ _pixel[ 0 ]] > barrier ) ) { // -??????@-@????@@
				return false;
			} // -??????@@@????@@
            if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // -??????@@@-???@@
                if( !(p[ _pixel[ 2 ] ] > barrier ) ) { // -?????-@@@-???@@
					return false;
				} // -?????@@@@-???@@
                if( !(p[ _pixel[ 3 ] ] > barrier ) ) { // -????-@@@@-???@@
					return false;
				} // -????@@@@@-???@@
                if( !(p[ _pixel[ 7 ] ] > barrier ) ) { // --???@@@@@-???@@
					return false;
				} // -@???@@@@@-???@@
                if( !(p[ _pixel[ 6 ] ] > barrier ) ) { // -@-??@@@@@-???@@
					return false;
				} // -@@??@@@@@-???@@
                if( !(p[ _pixel[ 5 ] ] > barrier ) ) { // -@@-?@@@@@-???@@
					return false;
				} // -@@@?@@@@@-???@@
                if( !(p[ _pixel[ 4 ] ] > barrier ) ) { // -@@@-@@@@@-???@@
					return false;
				} // -@@@@@@@@@-???@@
				return true;
			} // -??????@@@@???@@
            if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // -??????@@@@-??@@
                if( !(p[ _pixel[ 2 ] ] > barrier ) ) { // -?????-@@@@-??@@
					return false;
				} // -?????@@@@@-??@@
                if( !(p[ _pixel[ 6 ] ] > barrier ) ) { // -?-???@@@@@-??@@
					return false;
				} // -?@???@@@@@-??@@
                if( !(p[ _pixel[ 5 ] ] > barrier ) ) { // -?@-??@@@@@-??@@
					return false;
				} // -?@@??@@@@@-??@@
                if( !(p[ _pixel[ 4 ] ] > barrier ) ) { // -?@@-?@@@@@-??@@
					return false;
				} // -?@@@?@@@@@-??@@
                if( !(p[ _pixel[ 3 ] ] > barrier ) ) { // -?@@@-@@@@@-??@@
					return false;
				} // -?@@@@@@@@@-??@@
				return true;
			} // -??????@@@@@??@@
            if( !(p[ _pixel[ 12 ] ] > barrier ) ) { // -??????@@@@@-?@@
                if( !(p[ _pixel[ 5 ] ] > barrier ) ) { // -??-???@@@@@-?@@
					return false;
				} // -??@???@@@@@-?@@
                if( !(p[ _pixel[ 4 ] ] > barrier ) ) { // -??@-??@@@@@-?@@
					return false;
				} // -??@@??@@@@@-?@@
                if( !(p[ _pixel[ 3 ] ] > barrier ) ) { // -??@@-?@@@@@-?@@
					return false;
				} // -??@@@?@@@@@-?@@
                if( !(p[ _pixel[ 2 ] ] > barrier ) ) { // -??@@@-@@@@@-?@@
					return false;
				} // -??@@@@@@@@@-?@@
				return true;
			} // -??????@@@@@@?@@
            if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // -??????@@@@@@-@@
                if( !(p[ _pixel[ 4 ] ] > barrier ) ) { // -???-??@@@@@@-@@
					return false;
				} // -???@??@@@@@@-@@
                if( !(p[ _pixel[ 3 ] ] > barrier ) ) { // -???@-?@@@@@@-@@
					return false;
				} // -???@@?@@@@@@-@@
                if( !(p[ _pixel[ 2 ] ] > barrier ) ) { // -???@@-@@@@@@-@@
					return false;
				} // -???@@@@@@@@@-@@
				return true;
			} // -??????@@@@@@@@@
			return true;
		} // @?????????????@@
        if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // @????????????-@@
            if( !(p[ _pixel[ 2 ] ] > barrier ) ) { // @?????-??????-@@
				return false;
			} // @?????@??????-@@
            if( !(p[ _pixel[ 3 ] ] > barrier ) ) { // @????-@??????-@@
				return false;
			} // @????@@??????-@@
            if( !(p[ _pixel[ 4 ] ] > barrier ) ) { // @???-@@??????-@@
				return false;
			} // @???@@@??????-@@
            if( !(p[ _pixel[ 5 ] ] > barrier ) ) { // @??-@@@??????-@@
                if( !(p[ _pixel[ 12 ] ] > barrier ) ) { // @??-@@@?????--@@
					return false;
				} // @??-@@@?????@-@@
                if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // @??-@@@????-@-@@
					return false;
				} // @??-@@@????@@-@@
                if( !(p[ _pixel[ 1 ]] > barrier ) ) { // @??-@@@-???@@-@@
					return false;
				} // @??-@@@@???@@-@@
                if( !(p[ _pixel[ 0 ]] > barrier ) ) { // @??-@@@@-??@@-@@
					return false;
				} // @??-@@@@@??@@-@@
                if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // @??-@@@@@-?@@-@@
					return false;
				} // @??-@@@@@@?@@-@@
                if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // @??-@@@@@@-@@-@@
					return false;
				} // @??-@@@@@@@@@-@@
				return true;
			} // @??@@@@??????-@@
            if( !(p[ _pixel[ 6 ] ] > barrier ) ) { // @?-@@@@??????-@@
                if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // @?-@@@@????-?-@@
					return false;
				} // @?-@@@@????@?-@@
                if( !(p[ _pixel[ 1 ]] > barrier ) ) { // @?-@@@@-???@?-@@
					return false;
				} // @?-@@@@@???@?-@@
                if( !(p[ _pixel[ 0 ]] > barrier ) ) { // @?-@@@@@-??@?-@@
					return false;
				} // @?-@@@@@@??@?-@@
                if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // @?-@@@@@@-?@?-@@
					return false;
				} // @?-@@@@@@@?@?-@@
                if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // @?-@@@@@@@-@?-@@
					return false;
				} // @?-@@@@@@@@@?-@@
				return true;
			} // @?@@@@@??????-@@
            if( !(p[ _pixel[ 7 ] ] > barrier ) ) { // @-@@@@@??????-@@
                if( !(p[ _pixel[ 1 ]] > barrier ) ) { // @-@@@@@-?????-@@
					return false;
				} // @-@@@@@@?????-@@
                if( !(p[ _pixel[ 0 ]] > barrier ) ) { // @-@@@@@@-????-@@
					return false;
				} // @-@@@@@@@????-@@
                if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // @-@@@@@@@-???-@@
					return false;
				} // @-@@@@@@@@???-@@
                if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // @-@@@@@@@@-??-@@
					return false;
				} // @-@@@@@@@@@??-@@
				return true;
			} // @@@@@@@??????-@@
			return true;
		} // @????????????@@@
        if( !(p[ _pixel[ 7 ] ] > barrier ) ) { // @-???????????@@@
            if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // @-????????-??@@@
				return false;
			} // @-????????@??@@@
            if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // @-???????-@??@@@
				return false;
			} // @-???????@@??@@@
            if( !(p[ _pixel[ 0 ]] > barrier ) ) { // @-??????-@@??@@@
				return false;
			} // @-??????@@@??@@@
            if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // @-??????@@@-?@@@
                if( !(p[ _pixel[ 1 ]] > barrier ) ) { // @-?????-@@@-?@@@
					return false;
				} // @-?????@@@@-?@@@
                if( !(p[ _pixel[ 2 ] ] > barrier ) ) { // @-????-@@@@-?@@@
					return false;
				} // @-????@@@@@-?@@@
                if( !(p[ _pixel[ 6 ] ] > barrier ) ) { // @--???@@@@@-?@@@
					return false;
				} // @-@???@@@@@-?@@@
                if( !(p[ _pixel[ 5 ] ] > barrier ) ) { // @-@-??@@@@@-?@@@
					return false;
				} // @-@@??@@@@@-?@@@
                if( !(p[ _pixel[ 4 ] ] > barrier ) ) { // @-@@-?@@@@@-?@@@
					return false;
				} // @-@@@?@@@@@-?@@@
                if( !(p[ _pixel[ 3 ] ] > barrier ) ) { // @-@@@-@@@@@-?@@@
					return false;
				} // @-@@@@@@@@@-?@@@
				return true;
			} // @-??????@@@@?@@@
            if( !(p[ _pixel[ 12 ] ] > barrier ) ) { // @-??????@@@@-@@@
                if( !(p[ _pixel[ 1 ]] > barrier ) ) { // @-?????-@@@@-@@@
					return false;
				} // @-?????@@@@@-@@@
                if( !(p[ _pixel[ 5 ] ] > barrier ) ) { // @-?-???@@@@@-@@@
					return false;
				} // @-?@???@@@@@-@@@
                if( !(p[ _pixel[ 4 ] ] > barrier ) ) { // @-?@-??@@@@@-@@@
					return false;
				} // @-?@@??@@@@@-@@@
                if( !(p[ _pixel[ 3 ] ] > barrier ) ) { // @-?@@-?@@@@@-@@@
					return false;
				} // @-?@@@?@@@@@-@@@
                if( !(p[ _pixel[ 2 ] ] > barrier ) ) { // @-?@@@-@@@@@-@@@
					return false;
				} // @-?@@@@@@@@@-@@@
				return true;
			} // @-??????@@@@@@@@
			return true;
		} // @@???????????@@@
        if( !(p[ _pixel[ 12 ] ] > barrier ) ) { // @@??????????-@@@
            if( !(p[ _pixel[ 3 ] ] > barrier ) ) { // @@???-??????-@@@
				return false;
			} // @@???@??????-@@@
            if( !(p[ _pixel[ 4 ] ] > barrier ) ) { // @@??-@??????-@@@
				return false;
			} // @@??@@??????-@@@
            if( !(p[ _pixel[ 5 ] ] > barrier ) ) { // @@?-@@??????-@@@
				return false;
			} // @@?@@@??????-@@@
            if( !(p[ _pixel[ 6 ] ] > barrier ) ) { // @@-@@@??????-@@@
                if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // @@-@@@?????--@@@
					return false;
				} // @@-@@@?????@-@@@
                if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // @@-@@@????-@-@@@
					return false;
				} // @@-@@@????@@-@@@
                if( !(p[ _pixel[ 2 ] ] > barrier ) ) { // @@-@@@-???@@-@@@
					return false;
				} // @@-@@@@???@@-@@@
                if( !(p[ _pixel[ 1 ]] > barrier ) ) { // @@-@@@@-??@@-@@@
					return false;
				} // @@-@@@@@??@@-@@@
                if( !(p[ _pixel[ 0 ]] > barrier ) ) { // @@-@@@@@-?@@-@@@
					return false;
				} // @@-@@@@@@?@@-@@@
                if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // @@-@@@@@@-@@-@@@
					return false;
				} // @@-@@@@@@@@@-@@@
				return true;
			} // @@@@@@??????-@@@
			return true;
		} // @@??????????@@@@
        if( !(p[ _pixel[ 6 ] ] > barrier ) ) { // @@-?????????@@@@
            if( !(p[ _pixel[ 15 ] ] > barrier ) ) { // @@-??????-??@@@@
				return false;
			} // @@-??????@??@@@@
            if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // @@-??????@-?@@@@
				return false;
			} // @@-??????@@?@@@@
            if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // @@-??????@@-@@@@
				return false;
			} // @@-??????@@@@@@@
			return true;
		} // @@@?????????@@@@
        if( !(p[ _pixel[ 11 ] ] > barrier ) ) { // @@@????????-@@@@
            if( !(p[ _pixel[ 5 ] ] > barrier ) ) { // @@@-???????-@@@@
				return false;
			} // @@@@???????-@@@@
            if( !(p[ _pixel[ 4 ] ] > barrier ) ) { // @@@@-??????-@@@@
				return false;
			} // @@@@@??????-@@@@
			return true;
		} // @@@????????@@@@@
        if( !(p[ _pixel[ 5 ] ] > barrier ) ) { // @@@-???????@@@@@
            if( !(p[ _pixel[ 14 ] ] > barrier ) ) { // @@@-??????-@@@@@
				return false;
			} // @@@-??????@@@@@@
			return true;
		} // @@@@???????@@@@@
		return true;
	}

	const int ORB::_circularoffset[ 31 ] = {  3,  6,  8,  9, 10, 11, 12, 13, 13, 14, 14, 14, 15, 15, 15, 15,
											 15, 15, 15, 14, 14, 14, 13, 13, 12, 11, 10,  9,  8,  6,  3 };
#include "ORBPatterns.h"
}
