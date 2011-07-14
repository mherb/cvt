#include <cvt/math/Math.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/Resources.h>
#include <iostream>

using namespace cvt;

void pyrdownHalfHorizontal1u8_to_u16( uint16_t* dst, const uint8_t* src, size_t n )
{
	*dst++ =  ( ( ( uint16_t ) *( src + 1 ) ) << 2 ) + ( ( ( uint16_t ) *( src + 1 ) ) << 1 ) +
			  ( ( ( uint16_t ) *( src ) + ( uint16_t ) *( src + 2 ) ) << 2 ) +
			  ( ( ( uint16_t ) *( src + 3 ) ) << 1 );

	src += 3;
	if( n >= 6 ) {
		size_t n2 = ( n >> 1 ) - 2;

		while( n2-- ) {
			*dst++ = ( ( ( ( uint16_t ) *src ) << 2 ) + ( ( ( uint16_t ) *src ) << 1 ) +
					  ( ( ( uint16_t ) *( src + 1 ) ) << 2 ) + ( ( ( uint16_t ) *( src - 1 ) ) << 2 ) +
					  ( uint16_t ) *( src + 2 ) + ( uint16_t ) *( src - 2 ) );
			src += 2;
		}
	}

	if( n & 1 ) {
		*dst++ = ( ( ( uint16_t ) *src ) << 2 ) + ( ( ( uint16_t ) *src ) << 1 ) + ( ( ( uint16_t ) *( src - 2 ) ) << 1 ) +
			( ( ( uint16_t ) *( src + 1 ) ) << 2 ) + ( ( ( uint16_t ) *( src - 1 ) ) << 2 );
	} else {
		*dst++ = ( ( ( uint16_t ) *src ) << 2 ) +
			( ( ( uint16_t ) *src ) << 1 ) +
			( ( ( ( ( uint16_t ) *( src - 1 ) ) << 2 ) +
			   ( uint16_t ) *( src - 2 ) ) << 1 );

	}
}

void pyrdownHalfHorizontal1u8_to_u16_SSE2( uint16_t* dst, const uint8_t* src, size_t n )
{
	const __m128i mask = _mm_set1_epi16( 0xff00 );
	__m128i odd, even, even6, res;

	*dst++ =  ( ( ( uint16_t ) *( src + 1 ) ) << 2 ) + ( ( ( uint16_t ) *( src + 1 ) ) << 1 ) +
			  ( ( ( uint16_t ) *( src ) + ( uint16_t ) *( src + 2 ) ) << 2 ) +
			  ( ( ( uint16_t ) *( src + 3 ) ) << 1 );

	const uint8_t* end = src + n - 16;
	while( src < end ) {
		odd = _mm_loadu_si128( ( __m128i* ) src );
		even = _mm_srli_si128( _mm_and_si128( mask, odd ), 1 );
		odd = _mm_andnot_si128( mask, odd );

		odd = _mm_slli_epi16( odd, 2 );
		even6 = _mm_add_epi16( _mm_slli_epi16( even, 2 ),  _mm_slli_epi16( even, 1 ) );

		res = _mm_srli_si128( _mm_add_epi16( odd, even ), 2 );
		res = _mm_add_epi16( res, _mm_add_epi16( odd, even6 ) );
		res = _mm_add_epi16( res, _mm_slli_si128( even, 2 ) );
		res = _mm_srli_si128( res, 2 );

		_mm_storeu_si128( ( __m128i* ) dst, res );

		dst += 6;
		src += 12;
	}

	size_t n2 = ( ( n >> 1 ) - 2 ) % 6;
	src += 3;
	while( n2-- ) {
		*dst++ = ( ( ( ( uint16_t ) *src ) << 2 ) + ( ( ( uint16_t ) *src ) << 1 ) +
				  ( ( ( uint16_t ) *( src + 1 ) ) << 2 ) + ( ( ( uint16_t ) *( src - 1 ) ) << 2 ) +
				  ( uint16_t ) *( src + 2 ) + ( uint16_t ) *( src - 2 ) );
		src += 2;
	}

	if( n & 1 ) {
		*dst++ = ( ( ( uint16_t ) *src ) << 2 ) + ( ( ( uint16_t ) *src ) << 1 ) + ( ( ( uint16_t ) *( src - 2 ) ) << 1 ) +
			( ( ( uint16_t ) *( src + 1 ) ) << 2 ) + ( ( ( uint16_t ) *( src - 1 ) ) << 2 );
	} else {
		*dst++ = ( ( ( uint16_t ) *src ) << 2 ) +
			( ( ( uint16_t ) *src ) << 1 ) +
			( ( ( ( ( uint16_t ) *( src - 1 ) ) << 2 ) +
			   ( uint16_t ) *( src - 2 ) ) << 1 );
	}
}

void pyrdownHalfVerticalu16_to_u8( uint8_t* dst, uint16_t** rows, size_t n )
{
	uint16_t tmp;
	uint16_t* src1 = rows[ 0 ];
	uint16_t* src2 = rows[ 1 ];
	uint16_t* src3 = rows[ 2 ];
	uint16_t* src4 = rows[ 3 ];
	uint16_t* src5 = rows[ 4 ];

	while( n-- ) {
		tmp = *src1++ + *src5++ + ( ( *src2++ + *src4++ ) << 2 ) + 6 * *src3++;
		*dst++ = ( uint8_t ) ( tmp >> 8 );
	}
}

void pyrdownHalfVerticalu16_to_u8_SSE2( uint8_t* dst, uint16_t** rows, size_t n )
{
	uint16_t tmp;
	uint16_t* src1 = rows[ 0 ];
	uint16_t* src2 = rows[ 1 ];
	uint16_t* src3 = rows[ 2 ];
	uint16_t* src4 = rows[ 3 ];
	uint16_t* src5 = rows[ 4 ];
	__m128i r, t, zero;

	zero = _mm_setzero_si128();
	size_t n2 = n >> 3;
	while( n2-- ) {
		r = _mm_loadu_si128( ( __m128i* ) src1 );
		r = _mm_add_epi16( r, _mm_loadu_si128( ( __m128i* ) src5 ) );
		t = _mm_loadu_si128( ( __m128i* ) src2 );
		t = _mm_add_epi16( t, _mm_loadu_si128( ( __m128i* ) src4 ) );
		r = _mm_add_epi16( r, _mm_slli_epi16( t, 2 ) );
		t = _mm_loadu_si128( ( __m128i* ) src3 );
		t = _mm_add_epi16( _mm_slli_epi16( t, 2 ), _mm_slli_epi16( t, 1 ) );
		r = _mm_add_epi16( r, t );
		r = _mm_srli_epi16( r, 8 );
		r = _mm_packus_epi16( r, zero );
		_mm_storel_epi64( ( __m128i* ) dst, r );

		src1 += 8;
		src2 += 8;
		src3 += 8;
		src4 += 8;
		src5 += 8;
		dst  += 8;
	}

	n &= 0x7;
	while( n-- ) {
		tmp = *src1++ + *src5++ + ( ( *src2++ + *src4++ ) << 2 ) + 6 * *src3++;
		*dst++ = ( uint8_t ) ( tmp >> 8 );
	}
}

void u16_to_u8( uint8_t* dst, uint16_t* src, size_t n )
{
	while( n-- )
		*dst++ = ( *src++ ) >> 4;
}

void halfHoriz()
{
	Resources res;
	String file = res.find( "lena_g.png" );
	Image i;
	i.load( file );
	Image out( i.width() / 2, i.height() / 2, i.format() );

	size_t bstride = Math::pad16( i.width() / 2 );
	uint16_t buf[ bstride * 5 ];
	size_t sstride, dstride;
	uint8_t* src = i.map( &sstride );
	uint8_t* dst = out.map( &dstride );
	uint8_t* psrc = src;
	uint8_t* pdst = dst;

	uint16_t* rows[ 5 ];


	pyrdownHalfHorizontal1u8_to_u16( buf, psrc, i.width() );
	psrc += sstride;
	pyrdownHalfHorizontal1u8_to_u16( buf + bstride, psrc, i.width() );
	psrc += sstride;
	pyrdownHalfHorizontal1u8_to_u16( buf + 2 * bstride, psrc, i.width() );
	psrc += sstride;
	pyrdownHalfHorizontal1u8_to_u16( buf + 3 * bstride, psrc, i.width() );
	psrc += sstride;

	rows[ 0 ] = buf;
	rows[ 1 ] = buf;
	rows[ 2 ] = buf + bstride;
	rows[ 3 ] = buf + 2 * bstride;
	rows[ 4 ] = buf + 3 * bstride;

	pyrdownHalfVerticalu16_to_u8( pdst, rows, out.width() );
	pdst += dstride;

	rows[ 0 ] = buf + 4 * bstride;
	size_t h = out.height() - 2;
	while( h-- ) {
		uint16_t* tmp1 = rows[ 0 ];
		pyrdownHalfHorizontal1u8_to_u16( tmp1, psrc, i.width() );
		psrc += sstride;
		uint16_t* tmp2 = rows[ 1 ];
		pyrdownHalfHorizontal1u8_to_u16( tmp2, psrc, i.width() );
		psrc += sstride;

		rows[ 0 ] = rows[ 2 ];
		rows[ 1 ] = rows[ 3 ];
		rows[ 2 ] = rows[ 4 ];
		rows[ 3 ] = tmp1;
		rows[ 4 ] = tmp2;

		pyrdownHalfVerticalu16_to_u8_SSE2( pdst, rows, out.width() );
		pdst += dstride;
	}
	uint16_t* tmp1 = rows[ 0 ];
	if( i.height() & 1 )
		pyrdownHalfHorizontal1u8_to_u16( tmp1, psrc, i.width() );
	else
		tmp1 = rows[ 4 ];

	rows[ 0 ] = rows[ 2 ];
	rows[ 1 ] = rows[ 3 ];
	rows[ 2 ] = rows[ 4 ];
	rows[ 3 ] = tmp1;
	rows[ 4 ] = tmp1;
	pyrdownHalfVerticalu16_to_u8( pdst, rows, out.width() );

	i.unmap( src );
	out.unmap( dst );

	out.save( "pyrfun.png" );
}


/*void halfHoriz2()
{
	Resources res;
	String file = res.find( "lena_g.png" );
	Image i;
	i.load( file );
	Image out( i.width() / 2, i.height(), i.format() );

	uint16_t buf[ i.width() / 2 ];
	size_t sstride, dstride;
	uint8_t* src = i.map( &sstride );
	uint8_t* dst = out.map( &dstride );
	uint8_t* psrc = src;
	uint8_t* pdst = dst;
	for( size_t y = 0; y < i.height(); y++) {
		pyrdownHalfHorizontal1u8_to_u16_SSE2( buf, psrc, i.width() );
		u16_to_u8( pdst, buf, i.width() / 2 );
		psrc += sstride;
		pdst += dstride;
	}

	i.unmap( src );
	out.unmap( dst );

	out.save( "pyrfunsse.png" );
}*/


int main()
{

	halfHoriz();
//	halfHoriz2();

#define SIZE 6
	uint8_t src[ SIZE ];
	uint16_t dst[ SIZE / 2 ];

	for( int i = 0; i < SIZE; i++ )
		src[ i ] = i;//( uint8_t ) Math::rand( 0x0, 0xff );

	pyrdownHalfHorizontal1u8_to_u16( dst, src, SIZE );
	for( int i = 0; i < SIZE / 2; i++ ) {
		std::cout << dst[ i ] << std::endl;
		dst[ i ] = 0;
	}

	std::cout << std::endl;

	pyrdownHalfHorizontal1u8_to_u16_SSE2( dst, src, SIZE );
	for( int i = 0; i < SIZE / 2; i++ )
		std::cout << dst[ i ] << std::endl;

}
