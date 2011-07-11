#include <cvt/vision/ORB.h>

#include <algorithm>

namespace cvt {

	ORB::ORB( const Image& img, size_t octaves, float scalefactor, uint8_t cornerThreshold ) :
        _threshold( cornerThreshold )
	{
		float scale = 1.0f;
		IScaleFilterBilinear scaleFilter;

		_features.reserve( 512 );
		detect( img, scale );
		for( size_t i = 1; i < octaves; i++ ) {
			Image pyrimg;
			scale *=  scalefactor;
			img.scale( pyrimg, ( size_t )( img.width() * scale ), ( size_t )( img.height() * scale ), scaleFilter );
			detect( pyrimg, scale );
		}
        selectBestFeatures( 1500 );
	}

	void ORB::detect( const Image& img, float scale )
	{
        // detect the features for this level
        size_t featureIdx = _features.size();

        size_t stride;
        const uint8_t * ptr = img.map( &stride );
        detect9( ptr, stride, img.width(), img.height(), scale );
        img.unmap( ptr );

        IntegralImage iimg( img );
		const float* iimgptr = iimg.sumImage().map<float>( &stride );

        while( featureIdx < _features.size() ){
            centroidAngle( _features[ featureIdx ], iimgptr, stride );
            descriptor( _features[ featureIdx ], iimgptr, stride );
            featureIdx++;
        }

		iimg.sumImage().unmap<float>( iimgptr );
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
		feature.angle = Math::TWO_PI * -0.75f  - feature.angle;
		while( feature.angle < 0 )
			feature.angle += Math::TWO_PI;
		while( feature.angle > Math::TWO_PI )
			feature.angle -= Math::TWO_PI;

	}

	void ORB::descriptor( ORBFeature& feature, const float* iimgptr, size_t widthstep )
	{
		size_t index = ( size_t ) ( feature.angle * 60.0f / Math::TWO_PI );
		if( index >= 60 )
			index = 0;
		size_t x = ( size_t ) feature.pt.x;
		size_t y = ( size_t ) feature.pt.y;


#define ORBTEST( n ) ( IntegralImage::area( iimgptr, x + _patterns[ index ][ ( n ) * 2 ][ 0 ] - 2, y + _patterns[ index ][ ( n ) * 2 ][ 1 ] -2, 5, 5, widthstep ) < \
					   IntegralImage::area( iimgptr, x + _patterns[ index ][ ( n ) * 2 + 1 ][ 0 ] - 2, y + _patterns[ index ][ ( n ) * 2 + 1 ][ 1 ] -2, 5, 5, widthstep ) )

		for( int i = 0; i < 32; i++ ) {
			feature.desc[ i ] = 0;
			for( int k = 0; k < 8; k++ ) {
				feature.desc[ i ] <<= 1;
				feature.desc[ i ] |= ORBTEST( i * 8 + k );
			}
		}
		feature.pt /= feature.scale;
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

    void ORB::detect9( const uint8_t* im, size_t stride, size_t width, size_t height, float scale )
    {
		makeOffsets( stride );
        size_t h = height - _border;
        size_t w = width - _border;

        im += ( _border * stride + _border );

        int upperBound;
        int lowerBound;

		SIMD * simd = SIMD::instance();

		static const float harrisK = 0.04f;
		static const float harrisThreshold = 1e4f;

        for( size_t y = _border; y < h; y++ ){
            const uint8_t * curr = im;

            for( size_t x = _border; x < w; x++ ){
                lowerBound = *curr - _threshold;
                upperBound = *curr + _threshold;

                if( lowerBound && isDarkerCorner9( curr, lowerBound ) ) {
					float harris = simd->harrisResponse1u8( curr, stride, 4, 4, harrisK /* k from Pollefeys slides */ );
					if( harris > harrisThreshold )
						_features.push_back( ORBFeature( x, y, 0.0f, scale, harris ) );
                } else if( upperBound < 255 && isBrighterCorner9( curr, upperBound ) ) {
					float harris = simd->harrisResponse1u8( curr, stride, 4, 4, harrisK );
					if( harris > harrisThreshold )
                    _features.push_back( ORBFeature( x, y, 0.0f, scale, harris ) );
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
