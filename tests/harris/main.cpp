#include <cvt/util/SIMD.h>
#include <cvt/math/Math.h>

#include <stdio.h>

#include <cvt/util/SIMDDebug.h>

using namespace cvt;

#define	__horizontal_sum(r, rw) do { \
	rw = _mm_shuffle_epi32( r, _MM_SHUFFLE(1, 0, 3, 2)); \
	r = _mm_add_epi32(r, rw); \
	rw = _mm_shuffle_epi32( r, _MM_SHUFFLE(2, 3, 0, 1)); \
	r = _mm_add_epi32(r, rw); \
} while( 0 )

float harris9( uint8_t* ptr, size_t stride, float k = 0.04f )
{
	uint8_t* src = ptr - 4 * stride - 4 + 1;
	__m128i dx, dy, t1, t2, zero;
	__m128i ix, iy, ixy;
	float a, b, c;

	zero = _mm_setzero_si128();
	ix = iy = ixy = _mm_setzero_si128();

	for( int i = 0; i < 7; i++ ) {
		t1 = _mm_loadl_epi64( ( __m128i* ) src );
		t2 = _mm_loadl_epi64( ( __m128i* ) ( src + 2 * stride ) );
		t1 = _mm_unpacklo_epi8( t1, zero );
		t2 = _mm_unpacklo_epi8( t2, zero );
		dy = _mm_sub_epi16( t2, t1 );
		dy = _mm_slli_si128( dy, 2 );
		src += stride;

		t1 = _mm_madd_epi16( dy, dy );
		iy = _mm_add_epi32( iy, t1 );

		t1 = _mm_loadl_epi64( ( __m128i* ) ( src - 1 ) );
		t2 = _mm_loadl_epi64( ( __m128i* ) ( src ) );
		t1 = _mm_slli_si128( t1, 1 );
		t1 = _mm_unpacklo_epi8( t1, zero );
		t2 = _mm_unpacklo_epi8( t2, zero );
		dx = _mm_sub_epi16( t2, t1 );
		dx = _mm_srli_si128( dx, 2 ); // FIXME: use and
		dx = _mm_slli_si128( dx, 2 );

		t1 = _mm_madd_epi16( dx, dx );
		ix = _mm_add_epi32( ix, t1 );

		t1 = _mm_madd_epi16( dx, dy );
		ixy = _mm_add_epi32( ixy, t1 );
	}
	__horizontal_sum( ix, t1 );
	__horizontal_sum( iy, t1 );
	__horizontal_sum( ixy, t1 );

	a = ( float ) _mm_cvtsi128_si32( ix );
	b = ( float ) _mm_cvtsi128_si32( iy );
	c = ( float ) _mm_cvtsi128_si32( ixy );

	return ( a * b - 2.0f * c * c ) - ( k * Math::sqr(a + b) );
}

int main()
{
	srand( time( NULL ) );

	uint8_t bla[ 9 * 9 ];
	for( int y = 0; y < 9; y++ ) {
		for( int x = 0; x < 9; x++ ) {
			bla[ y * 9 + x ] = ( uint8_t ) Math::rand( 0x0, 0xff);
			std::cout << " " << ( int ) bla[ y * 9 + x ];
		}
		std::cout << std::endl;
	}



	float harris = SIMD::instance()->harrisResponse1u8( bla + 4 * 9 + 4, 9, 4, 4, 0.04f );
	std::cout << harris << std::endl;
	harris = harris9( bla + 4 * 9 + 4, 9, 0.04f );
	std::cout << harris << std::endl;
}
