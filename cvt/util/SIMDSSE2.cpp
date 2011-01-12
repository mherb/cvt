#include <cvt/util/SIMDSSE2.h>

#include <xmmintrin.h>
#include <emmintrin.h>

namespace cvt {
	void SIMDSSE2::Conv_UYVYu8_to_RGBAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const
		/* convert RGBA to YUV422 */
	{
		const __m128i Y2RGB = _mm_set_epi16(1192,0,1192,0,1192,0,1192,0);
		const __m128i UV2R  = _mm_set_epi16(1634, 0, 1634, 0,
											1634, 0, 1634, 0);
		const __m128i UV2G  = _mm_set_epi16( -832, -401, -832, -401,
											 -832, -401, -832, -401);
		const __m128i UV2B  = _mm_set_epi16(0, 2066, 0, 2066,
											0, 2066, 0, 2066);
		const __m128i uvoffset = _mm_set1_epi16( 128 );
		const __m128i yoffset = _mm_set1_epi16( 16 );
		const __m128i  A32  = _mm_set_epi32(255, 255, 255, 255);
		const __m128i mask= _mm_set_epi8( 0xff, 0x0, 0xff, 0x0, 0xff, 0x0, 0xff, 0x0,
										  0xff, 0x0, 0xff, 0x0, 0xff, 0x0, 0xff, 0x0 );

		__m128i uyvy;
		__m128i UV, YZ, Y, Z;
		__m128i UV_R, UV_G, UV_B;
		__m128i R, G, B, A;
		__m128i RB0, RB1, GA0, GA1;

		const int shuffle =  _MM_SHUFFLE(3, 1, 2, 0);

		int i= n >> 3; /* we do 2*128bit per cycle: this is 2*4*32bit == 8 pixels */
		while( i-- ){
			uyvy = _mm_loadu_si128( ( __m128i* ) src );
			src += 16;
			/* u0 y0 v0 z0 u1 y1 v1 z1 u2 y2 v2 z2 u3 y3 v3 z3 */

			UV = _mm_and_si128( mask, uyvy );
			UV = _mm_srli_si128( UV, 1 );
			UV = _mm_sub_epi16( UV, yoffset );
			UV = _mm_shuffle_epi32( UV, shuffle);

			YZ = _mm_andnot_si128( mask, uyvy );
			YZ = _mm_sub_epi16( YZ, uvoffset );
			YZ = _mm_shuffle_epi32( YZ, shuffle);

			__m128i tmp = UV;
			UV = YZ;
			YZ = tmp;

			Z = _mm_madd_epi16(YZ, Y2RGB);                    /* Z0' Z2' Z1' Z3' */
			Y = _mm_madd_epi16(YZ, _mm_srli_si128(Y2RGB, 2)); /* Y0' Y2' Y1' Y3' */

			UV_R = _mm_madd_epi16(UV, UV2R);
			UV_G = _mm_madd_epi16(UV, UV2G);
			UV_B = _mm_madd_epi16(UV, UV2B);

			R  = _mm_srai_epi32(_mm_add_epi32(Y, UV_R), 10);
			G  = _mm_srai_epi32(_mm_add_epi32(Y, UV_G), 10);
			B  = _mm_srai_epi32(_mm_add_epi32(Y, UV_B), 10);

			RB0 = _mm_packs_epi32(R, B);
			GA0 = _mm_packs_epi32(G, A32);

			R  = _mm_srai_epi32(_mm_add_epi32(Z, UV_R), 10);
			G  = _mm_srai_epi32(_mm_add_epi32(Z, UV_G), 10);
			B  = _mm_srai_epi32(_mm_add_epi32(Z, UV_B), 10);

			RB1 = _mm_packs_epi32(R, B);
			GA1 = _mm_packs_epi32(G, A32);

			R  = _mm_unpacklo_epi16(RB0, RB1);  /* R0 R1 R4 R5 R2 R3 R6 R7 */
			R  = _mm_shuffle_epi32 (R, shuffle);/* R0 R1 R2 R3 R4 R5 R6 R7 */
			B  = _mm_unpackhi_epi16(RB0, RB1);
			B  = _mm_shuffle_epi32 (B, shuffle);
			G  = _mm_unpacklo_epi16(GA0, GA1);
			G  = _mm_shuffle_epi32 (G, shuffle);
			A  = _mm_unpackhi_epi16(GA0, GA1); /* no need to shuffle, since A0=A1=...=255 */

			RB0= _mm_unpacklo_epi16(R, B);
			RB1= _mm_unpackhi_epi16(R, B);
			RB0= _mm_packus_epi16(RB0, RB1); /* R0 B0 R1 B1 R2 B2 R3 B3 R4 B4 R5 B5 R6 B6 R7 B7 */

			GA0= _mm_unpacklo_epi16(G, A);
			GA1= _mm_unpackhi_epi16(G, A);
			GA0= _mm_packus_epi16(GA0, GA1);

			_mm_stream_si128( ( __m128i* ) dst,  _mm_unpacklo_epi8(RB0, GA0));
			dst += 16;
			_mm_stream_si128( ( __m128i* ) dst,  _mm_unpackhi_epi8(RB0, GA0));
			dst += 16;
		}
	}
#if 0
	{
		__mm128i in, rb, r, g, b;
		size_t n2 = n >> 2;

		__m128i yoff  = _mm_set1_epi16( 128 );
		__m128i uvoff = _mm_set1_epi16(  16 );

		__m128i coeffy  = _mm_set1_epi16( 1192 );
		__m128i coeffg  = _mm_set_epi16( 401, 832, 401, 832, 401, 832, 401, 832 );
		__m128i coeffrb  = _mm_set_epi16( 1634, 2066, 1634, 2066, 1634, 2066, 1634, 2066 );
		__m128i mask  = _mm_set_epi16( 0, 0xffff, 0, 0xffff, 0, 0xffff, 0, 0xffff );
		__m128i zero  = _mm_setzero_si128();

		while( n2-- ) {
			in = _mm_loadlu_si128( ( __m128i* ) src );
			src += 16;

			__m128i y = _mm_unpackhi_epi8( in, zero );
			__m128i uv = _mm_unpacklo_epi8( in, zero );

			y = _mm_sub_epi16( y, yoff );
			uv = _mm_sub_epi16( uv, uvoff );

			y = _mm_mullo_epi16( y, coeffy );
			y = _mm_srai_epi16( y, 10 );
//			y = _mm_packus_epi16( y, zero );

			g = _mm_madd_epi16( uv, coeffg );
			g = _mm_srai_epi32( g, 10 );
			g = _mm_packs_epi32( g, zero );
//			g = _mm_sub_epi16( y, g );
//			g = _mm_packus_epi16( g, zero );

			rb = _mm_mullo_epi16( uv, coeffrb );
			rb = _mm_srai_epi16( rb, 10 );
//			r  = _
//			rb = _mm_packus_epi16( rb, zero );


		}
	}
#endif
}
