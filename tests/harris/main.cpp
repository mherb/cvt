#include <cvt/util/SIMD.h>
#include <cvt/math/Math.h>

#include <stdio.h>

using namespace cvt;
typedef union {
	__m128   m128;
	__m128i  m128i;
	float	 f[ 4 ];
	uint8_t  u8[ 16 ];
	int8_t	 i8[ 16 ];
	uint16_t u16[ 8 ];
	int16_t  i16[ 8 ];
	uint32_t u32[ 4 ];
	int32_t  i32[ 4 ];
} SIMD128;

void SIMD128_print_f( SIMD128* x )
{
	printf("%f %f %f %f\n", x->f[ 0 ], x->f[ 1 ], x->f[ 2 ], x->f[ 3 ] );
}

void SIMD128_print_u32( SIMD128* x )
{
	printf("0x%0X 0x%0X 0x%0X 0x%0X\n", x->u32[ 0 ], x->u32[ 1 ], x->u32[ 2 ], x->u32[ 3 ] );
}

void SIMD128_print_i32( SIMD128* x )
{
	printf("0x%0X 0x%0X 0x%0X 0x%0X\n", x->i32[ 0 ], x->i32[ 1 ], x->i32[ 2 ], x->i32[ 3 ] );
}

void SIMD128_print_u16( SIMD128* x )
{
	printf("0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X\n", x->u16[ 0 ], x->u16[ 1 ], x->u16[ 2 ], x->u16[ 3 ],
																x->u16[ 4 ], x->u16[ 5 ], x->u16[ 6 ], x->u16[ 7 ] );
}

void SIMD128_print_i16( SIMD128* x )
{
	printf("%d %d %d %d %d %d %d %d\n", x->i16[ 0 ], x->i16[ 1 ], x->i16[ 2 ], x->i16[ 3 ],
																x->i16[ 4 ], x->i16[ 5 ], x->i16[ 6 ], x->i16[ 7 ] );
}

void SIMD128_print_u8( SIMD128* x )
{
	printf("0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X\n",
		   x->u8[ 0 ], x->u8[ 1 ], x->u8[ 2 ], x->u8[ 3 ],
		   x->u8[ 4 ], x->u8[ 5 ], x->u8[ 6 ], x->u8[ 7 ],
		   x->u8[ 8 ], x->u8[ 9 ], x->u8[ 10 ], x->u8[ 11 ],
		   x->u8[ 12 ], x->u8[ 13 ], x->u8[ 14 ], x->u8[ 15 ] );
}

void SIMD128_print_i8( SIMD128* x )
{
	printf("0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X 0x%0X\n",
		   x->i8[ 0 ], x->i8[ 1 ], x->i8[ 2 ], x->i8[ 3 ],
		   x->i8[ 4 ], x->i8[ 5 ], x->i8[ 6 ], x->i8[ 7 ],
		   x->i8[ 8 ], x->i8[ 9 ], x->i8[ 10 ], x->i8[ 11 ],
		   x->i8[ 12 ], x->i8[ 13 ], x->i8[ 14 ], x->i8[ 15 ] );
}

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
