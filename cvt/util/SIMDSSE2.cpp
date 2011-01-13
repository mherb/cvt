#include <cvt/util/SIMDSSE2.h>

#include <xmmintrin.h>
#include <emmintrin.h>

namespace cvt
{
	void SIMDSSE2::Conv_UYVYu8_to_RGBAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const
	/* convert RGBA to YUV422 */
	{
		const __m128i Y2RGB = _mm_set_epi16( 1192, 0, 1192, 0, 1192, 0, 1192, 0 );
		const __m128i UV2R  = _mm_set_epi16( 1634, 0, 1634, 0, 1634, 0, 1634, 0 );
		const __m128i UV2G  = _mm_set_epi16( -832, -401, -832, -401, -832, -401, -832, -401 );
		const __m128i UV2B  = _mm_set_epi16( 0, 2066, 0, 2066, 0, 2066, 0, 2066 );
		const __m128i UVOFFSET = _mm_set1_epi16( 128 );
		const __m128i YOFFSET = _mm_set1_epi16( 16 );
		const __m128i A32  = _mm_set1_epi32( 0xff );
		const __m128i mask = _mm_set1_epi16( 0xff00 );

		__m128i uyvy;
		__m128i uv, yz, y, z;
		__m128i uvR, uvG, uvB;
		__m128i r, g, b, a;
		__m128i RB0, RB1, GA0, GA1;

		int i = n >> 3;
		while( i-- ) {
			uyvy = _mm_load_si128( ( __m128i* ) src );
			src += 16;
			/* U0 Y0 V0 Z0 U1 Y1 V1 Z1 U2 Y2 V2 Z2 U3 Y3 V3 Z3 */

			yz = _mm_and_si128( mask, uyvy );
			yz = _mm_srli_si128( yz, 1 );
			yz = _mm_sub_epi16( yz, YOFFSET );  /* Y0 Z0 Y1 Z1 ...  */

			uv = _mm_andnot_si128( mask, uyvy );
			uv = _mm_sub_epi16( uv, UVOFFSET ); /* U0 V0 U1 V1 ... */

			z = _mm_madd_epi16( yz, Y2RGB );                      /* Z0 Z1 Z2 Z3 */
			y = _mm_madd_epi16( yz, _mm_srli_si128( Y2RGB, 2 ) ); /* Y0 Y1 Y2 Y3 */

			uvR = _mm_madd_epi16( uv, UV2R );
			uvG = _mm_madd_epi16( uv, UV2G );
			uvB = _mm_madd_epi16( uv, UV2B );

			r  = _mm_srai_epi32( _mm_add_epi32( y, uvR ), 10 );
			g  = _mm_srai_epi32( _mm_add_epi32( y, uvG ), 10 );
			b  = _mm_srai_epi32( _mm_add_epi32( y, uvB ), 10 );

			RB0 = _mm_packs_epi32( r, b );
			GA0 = _mm_packs_epi32( g, A32 );

			r  = _mm_srai_epi32( _mm_add_epi32( z, uvR ), 10 );
			g  = _mm_srai_epi32( _mm_add_epi32( z, uvG ), 10 );
			b  = _mm_srai_epi32( _mm_add_epi32( z, uvB ), 10 );

			RB1 = _mm_packs_epi32( r, b );
			GA1 = _mm_packs_epi32( g, A32 );

			r  = _mm_unpacklo_epi16( RB0, RB1 );
			b  = _mm_unpackhi_epi16( RB0, RB1 );
			g  = _mm_unpacklo_epi16( GA0, GA1 );
			a  = _mm_unpackhi_epi16( GA0, GA1 );

			RB0 = _mm_unpacklo_epi16( r, b );
			RB1 = _mm_unpackhi_epi16( r, b );
			RB0 = _mm_packus_epi16( RB0, RB1 );

			GA0 = _mm_unpacklo_epi16( g, a );
			GA1 = _mm_unpackhi_epi16( g, a );
			GA0 = _mm_packus_epi16( GA0, GA1 );

			_mm_stream_si128( ( __m128i* ) dst,  _mm_unpacklo_epi8( RB0, GA0 ) );
			dst += 16;
			_mm_stream_si128( ( __m128i* ) dst,  _mm_unpackhi_epi8( RB0, GA0 ) );
			dst += 16;
		}
	}

	void SIMDSSE2::Conv_UYVYu8_to_BGRAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const
	/* convert RGBA to YUV422 */
	{
		const __m128i Y2RGB = _mm_set_epi16( 1192, 0, 1192, 0, 1192, 0, 1192, 0 );
		const __m128i UV2R  = _mm_set_epi16( 1634, 0, 1634, 0, 1634, 0, 1634, 0 );
		const __m128i UV2G  = _mm_set_epi16( -832, -401, -832, -401, -832, -401, -832, -401 );
		const __m128i UV2B  = _mm_set_epi16( 0, 2066, 0, 2066, 0, 2066, 0, 2066 );
		const __m128i UVOFFSET = _mm_set1_epi16( 128 );
		const __m128i YOFFSET = _mm_set1_epi16( 16 );
		const __m128i A32  = _mm_set1_epi32( 0xff );
		const __m128i mask = _mm_set1_epi16( 0xff00 );

		__m128i uyvy;
		__m128i uv, yz, y, z;
		__m128i uvR, uvG, uvB;
		__m128i r, g, b, a;
		__m128i RB0, RB1, GA0, GA1;

		int i = n >> 3;
		while( i-- ) {
			uyvy = _mm_load_si128( ( __m128i* ) src );
			src += 16;
			/* U0 Y0 V0 Z0 U1 Y1 V1 Z1 U2 Y2 V2 Z2 U3 Y3 V3 Z3 */

			yz = _mm_and_si128( mask, uyvy );
			yz = _mm_srli_si128( yz, 1 );
			yz = _mm_sub_epi16( yz, YOFFSET );  /* Y0 Z0 Y1 Z1 ...  */

			uv = _mm_andnot_si128( mask, uyvy );
			uv = _mm_sub_epi16( uv, UVOFFSET ); /* U0 V0 U1 V1 ... */

			z = _mm_madd_epi16( yz, Y2RGB );                      /* Z0 Z1 Z2 Z3 */
			y = _mm_madd_epi16( yz, _mm_srli_si128( Y2RGB, 2 ) ); /* Y0 Y1 Y2 Y3 */

			uvR = _mm_madd_epi16( uv, UV2R );
			uvG = _mm_madd_epi16( uv, UV2G );
			uvB = _mm_madd_epi16( uv, UV2B );

			r  = _mm_srai_epi32( _mm_add_epi32( y, uvR ), 10 );
			g  = _mm_srai_epi32( _mm_add_epi32( y, uvG ), 10 );
			b  = _mm_srai_epi32( _mm_add_epi32( y, uvB ), 10 );

			RB0 = _mm_packs_epi32( r, b );
			GA0 = _mm_packs_epi32( g, A32 );

			r  = _mm_srai_epi32( _mm_add_epi32( z, uvR ), 10 );
			g  = _mm_srai_epi32( _mm_add_epi32( z, uvG ), 10 );
			b  = _mm_srai_epi32( _mm_add_epi32( z, uvB ), 10 );

			RB1 = _mm_packs_epi32( r, b );
			GA1 = _mm_packs_epi32( g, A32 );

			r  = _mm_unpacklo_epi16( RB0, RB1 );
			b  = _mm_unpackhi_epi16( RB0, RB1 );
			g  = _mm_unpacklo_epi16( GA0, GA1 );
			a  = _mm_unpackhi_epi16( GA0, GA1 );

			RB0 = _mm_unpacklo_epi16( b, r );
			RB1 = _mm_unpackhi_epi16( b, r );
			RB0 = _mm_packus_epi16( RB0, RB1 );

			GA0 = _mm_unpacklo_epi16( g, a );
			GA1 = _mm_unpackhi_epi16( g, a );
			GA0 = _mm_packus_epi16( GA0, GA1 );

			_mm_stream_si128( ( __m128i* ) dst,  _mm_unpacklo_epi8( RB0, GA0 ) );
			dst += 16;
			_mm_stream_si128( ( __m128i* ) dst,  _mm_unpackhi_epi8( RB0, GA0 ) );
			dst += 16;
		}
	}

}
