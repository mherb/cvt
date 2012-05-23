#include <cvt/util/SIMD.h>
#include <cvt/util/SIMDDebug.h>
#include <cvt/util/Util.h>

#include <cvt/math/Math.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/IMapScoped.h>

using namespace cvt;

void mosaic_image( Image& out, const Image& in )
{
	if( in.format() != IFormat::RGBA_UINT8 ) {
		throw CVTException( "Only RGBA_UINT8 supported!" );
	}

	out.reallocate( in.width(), in.height(), IFormat::BAYER_RGGB_UINT8 );
	const uint32_t* src;
	const uint8_t	*smap;
	uint8_t* dst;
    uint8_t	*dmap;
	size_t sstride, dstride;

	smap = in.map( &sstride );
	dmap = out.map( &dstride );

	src = ( const uint32_t* ) smap;
	dst = dmap;

	size_t h = in.height();
	for( size_t y = 0; y < h; y++ ) {
		size_t w = in.width() >> 1;
		if( y & 1 ) {
		    // ODD
			while( w-- ) {
				*dst++ = ( ( *src++ ) >> 8 ) & 0xff;
				*dst++ = ( ( *src++ ) >> 16 ) & 0xff;
			}
		} else {
			// EVEN
			while( w-- ) {
				*dst++ = ( *src++ ) & 0xff;
				*dst++ = ( ( *src++ ) >> 8 ) & 0xff;
			}

		}
	}

	in.unmap( smap );
	out.unmap( dmap );
}

void sse2_debayer_EVEN_RGGBu8_GRAYu8( uint32_t* _dst, const uint32_t* src1, const uint32_t* src2, const uint32_t* src3, const size_t width )
	{
		size_t n = width >> 4;
        const __m128i zero = _mm_setzero_si128();
		const __m128i maskODD = _mm_set1_epi16( 0xff00 );
		const __m128i maskEVEN = _mm_set1_epi16( 0xff );
		const __m128i rlum = _mm_set1_epi16( 0x36 );
		const __m128i glum = _mm_set1_epi16( 0xB7 );
		const __m128i blum = _mm_set1_epi16( 0x12 );
		uint8_t* dst = ( uint8_t* ) _dst;

		__m128i b1, b2, g1, g2, g3, r1, t, t2;

		while( n-- ) {
			t = _mm_loadu_si128( ( __m128i * ) src1 );
			g1 = _mm_and_si128( t, maskEVEN );
			b1 = _mm_srli_si128( _mm_and_si128( t, maskODD ), 1 );

			t = _mm_loadu_si128( ( __m128i * ) src2 );
			r1 = _mm_and_si128( t, maskEVEN );
			g2 = _mm_srli_si128( _mm_and_si128( t, maskODD ), 1 );

			t = _mm_loadu_si128( ( __m128i * ) src3 );
			g3 = _mm_and_si128( t, maskEVEN );
			b2 = _mm_srli_si128( _mm_and_si128( t, maskODD ), 1 );

			b1 = _mm_add_epi16( b1, b2 );
			g1 = _mm_add_epi16( g1, g3 );

			t = _mm_mullo_epi16( r1, rlum );
			t = _mm_add_epi16( t, _mm_srli_epi16( _mm_mullo_epi16( _mm_add_epi16( b1, _mm_srli_si128( b1, 2 ) ), blum ), 2 ) );
			t = _mm_add_epi16( t, _mm_mullo_epi16( _mm_srli_epi16( _mm_add_epi16( g1, _mm_slli_si128( _mm_add_epi16( g2, _mm_srli_si128( g2, 2 ) ), 2 ) ), 2 ), glum ) );

			SIMD_print_u16hex( _mm_mullo_epi16( _mm_srli_epi16( _mm_add_epi16( g1, _mm_slli_si128( _mm_add_epi16( g2, _mm_srli_si128( g2, 2 ) ), 2 ) ), 2 ), glum ) );
			SIMD_print_u16hex( t );

			SIMD_print_u16hex( g1 );
			SIMD_print_u16hex( g2 );
			SIMD_print_u16hex( b1 );
			SIMD_print_u16hex( r1 );
			SIMD_print_u16hex( _mm_add_epi16( r1, _mm_srli_si128( r1, 2 ) ) );
			printf("------\n");

			SIMD_print_u16hex( t );
			t = _mm_srli_epi16( t, 8 );
	//		SIMD_print_u16hex( t );

			t2 = _mm_srli_epi16( _mm_mullo_epi16( r1, rlum ), 8 );
			t2 = _mm_add_epi16( t2, _mm_srli_epi16( _mm_mullo_epi16( _mm_add_epi16( b1, _mm_slli_si128( b1, 2 ) ), blum ), 10 ) );
			t2 = _mm_add_epi16( t2, _mm_srli_epi16( _mm_mullo_epi16( _mm_add_epi16( g1, _mm_add_epi16( g2, _mm_slli_si128( g2, 2 ) ) ), glum ), 10 ) );

	//		SIMD_print_u16hex( t2 );

		//	t = _mm_slli_si128( t, 1 );
	//		SIMD_print_u16hex(  _mm_or_si128( t, t2 ) );
	//		std::cout << std::endl;
			t2 = zero;
			_mm_storeu_si128( ( __m128i * ) ( dst ), _mm_or_si128( t, t2 ) );

			dst += 16;
			src1 += 4;
			src2 += 4;
			src3 += 4;
		}
	}

#define BAYER_RGGB_R1( x ) ( ( x ) & 0xff )
#define BAYER_RGGB_R2( x ) ( ( ( x ) & 0xff0000 ) >> 16 )
#define BAYER_RGGB_EVEN_G1( x ) ( ( x ) & 0xff )
#define BAYER_RGGB_EVEN_G2( x ) ( ( ( x ) & 0xff0000 ) >> 16 )
#define BAYER_RGGB_ODD_G1( x ) ( ( ( x ) & 0xff00 ) >> 8 )
#define BAYER_RGGB_ODD_G2( x ) ( ( x ) >> 24 )
#define BAYER_RGGB_B1( x ) ( ( ( x ) & 0xff00 ) >> 8 )
#define BAYER_RGGB_B2( x ) ( ( x ) >> 24 )

#define BAYER_MIX2( x, y ) ( ( ( x ) + ( y ) ) >> 1 )
#define BAYER_MIX4( x, y, z, w  ) ( ( ( x ) + ( y ) + ( z ) + ( w ) ) >> 2 )

#define BAYER_MIX4_NODIV( x, y, z, w  ) ( ( ( ( x ) + ( y ) + ( z ) + ( w ) ) ) << 1 )

#define BAYER_MIX2_NODIV( x, y ) ( ( ( x ) + ( y ) ) << 2 )

#define BAYER_GRAD5_4( x, y, z, w, v  ) ( ( ( ( int )( ( x ) << 2 ) - ( int )( y ) - ( int )( z ) - ( int )( w ) - ( int )( v ) ) ) )

#define BAYER_GRAD5_6( x, y, z, w, v  ) ( ( ( ( int )( ( ( x ) << 2 ) + ( ( x ) << 1 ) ) - \
									    ( ( ( int )( y ) + ( int )( z ) + ( int )( w ) + ( int )( v ) ) + ( ( ( int )( y ) + ( int )( z ) + ( int )( w ) + ( int )( v ) ) >> 1 ) ) ) ) )
#define BAYER_GRAD9_5( x, y, z, w, v, m, n, o, p  ) ( ( ( ( int )( ( ( x ) << 2 ) + ( ( x ) ) ) - \
									    ( ( ( int )( y ) + ( int )( z ) + ( int )( w ) + ( int )( v ) + ( int ) ( m ) + ( int ) ( n ) ) ) + ( ( ( int )( o ) + ( int )( p ) ) >> 1 ) ) ) )

#define HQPTR(x,off) ( ( int ) *((x)+off) )

#define BAYER_HQ_G_AT_R( p1, p2, p3, p4, p5 ) ( ( ( ( HQPTR(p3,-1) + HQPTR(p3,1) + HQPTR(p2,0) + HQPTR(p4,0) ) << 1 ) \
												 + ( HQPTR(p3,0) << 2 ) - HQPTR(p1,0) - HQPTR(p5,0) - HQPTR(p3,-2) - HQPTR(p3,2) ) >> 3 )

#define BAYER_HQ_G_AT_B( p1, p2, p3, p4, p5 ) BAYER_HQ_G_AT_R( p1, p2, p3, p4, p5 )


#define BAYER_HQ_R_AT_G_EVEN( p1, p2, p3, p4, p5 ) ( ( ( ( HQPTR(p3,-1) + HQPTR(p3, 1) ) << 2 ) + ( HQPTR(p3,0) << 2 ) + HQPTR(p3,0) \
													  - HQPTR(p2,-1) - HQPTR(p2,+1) - HQPTR(p4,-1) - HQPTR(p4,1) - HQPTR(p3,-2) - HQPTR(p3,+2) + \
													( ( HQPTR(p1,0) + HQPTR(p5,0) ) >> 1 ) ) >> 3 )

#define BAYER_HQ_B_AT_G_EVEN( p1, p2, p3, p4, p5 ) ( ( ( ( HQPTR(p2,0) + HQPTR(p4, 0) ) << 2 ) + ( HQPTR(p3,0) << 2 ) + HQPTR(p3,0) \
													  - HQPTR(p2,-1) - HQPTR(p2,+1) - HQPTR(p4,-1) - HQPTR(p4,1) - HQPTR(p1,0) - HQPTR(p5,0) + \
													( ( HQPTR(p3,-2) + HQPTR(p3,+2) ) >> 1 ) ) >> 3 )


#define BAYER_HQ_B_AT_R_EVEN( p1, p2, p3, p4, p5 ) ( ( ( ( HQPTR(p2,-1) + HQPTR(p2, 1) + HQPTR(p4,-1) + HQPTR(p4,1) ) << 1 ) + ( ( HQPTR(p3,0) << 2 ) + ( HQPTR(p3,0) << 1 ) ) \
													- ( HQPTR(p3,-2) + HQPTR(p3,+2) + HQPTR(p1,0) + HQPTR(p5,0) ) \
													- ( ( HQPTR(p3,-2) + HQPTR(p3,+2) + HQPTR(p1,0) + HQPTR(p5,0) ) >> 1  ) ) >> 3 )

#define BAYER_HQ_R_AT_G_ODD( p1, p2, p3, p4, p5 ) BAYER_HQ_B_AT_G_EVEN( p1, p2, p3, p4, p5 )

#define BAYER_HQ_B_AT_G_ODD( p1, p2, p3, p4, p5 ) BAYER_HQ_R_AT_G_EVEN( p1, p2, p3, p4, p5 )

#define BAYER_HQ_R_AT_B_ODD( p1, p2, p3, p4, p5 ) BAYER_HQ_B_AT_R_EVEN( p1, p2, p3, p4, p5 )

void debayerhq_even( uint32_t* dst, const uint8_t* src1, const uint8_t* src2,
					 const uint8_t* src3, const uint8_t* src4, const uint8_t* src5, const size_t n ) 
{
	uint32_t v;

	v = 0xff000000;
	v |= *src3; // RED
	v |= BAYER_MIX4( HQPTR(src3,1), HQPTR(src3,1), HQPTR(src2,0), HQPTR(src4,0) ) << 8; // GREEN
	v |= BAYER_MIX2( HQPTR(src2,1), HQPTR(src4,1) ) << 16; // BLUE
	*dst++ = v;
	src1++;
	src2++;
	src3++;
	src4++;
	src5++;

	v = 0xff000000;
	v |= BAYER_MIX2( HQPTR(src3,1), HQPTR(src3,-1) ); // RED
	v |= *src3 << 8; // GREEN
	v |= BAYER_MIX2( HQPTR(src2,0), HQPTR(src4,0) ) << 16; // BLUE
	*dst++ = v;
	src1++;
	src2++;
	src3++;
	src4++;
	src5++;

	size_t i = ( n - 4 ) >> 1;
	while( i-- ) {
		v = 0xff000000;
		v |= *src3; // RED
		v |= Math::clamp( BAYER_HQ_G_AT_R( src1, src2, src3, src4, src5 ), 0x0, 0xff ) << 8; // GREEN
		v |= Math::clamp( BAYER_HQ_B_AT_R_EVEN( src1, src2, src3, src4, src5 ), 0x0, 0xff ) << 16; // BLUE
		*dst++ = v;
		src1++;
		src2++;
		src3++;
		src4++;
		src5++;

		v = 0xff000000;
		v |= Math::clamp( BAYER_HQ_R_AT_G_EVEN( src1, src2, src3, src4, src5 ), 0x0, 0xff ); // RED
		v |= ( *src3 ) << 8; // GREEN
		v |= Math::clamp( BAYER_HQ_B_AT_G_EVEN( src1, src2, src3, src4, src5 ), 0x0, 0xff ) << 16; // BLUE
		*dst++ = v;
		src1++;
		src2++;
		src3++;
		src4++;
		src5++;
	}

	v = 0xff000000;
	v |= *src3; // RED
	v |= BAYER_MIX4( HQPTR(src3,-1), HQPTR(src3,1), HQPTR(src2,0), HQPTR(src4,0) ) << 8; // GREEN
	v |= BAYER_MIX4( HQPTR(src2,-1), HQPTR(src4,-1), HQPTR(src2,1), HQPTR(src4,1) ) << 16; // BLUE
	*dst++ = v;
	src1++;
	src2++;
	src3++;
	src4++;
	src5++;

	v = 0xff000000;
	v |= *( src3 - 1 ); // RED
	v |= *src3 << 8; // GREEN
	v |= BAYER_MIX2( HQPTR(src2,0), HQPTR(src4,0) ) << 16; // BLUE
	*dst++ = v;
	src1++;
	src2++;
	src3++;
	src4++;
	src5++;
}

void debayerhq_odd( uint32_t* dst, const uint8_t* src1, const uint8_t* src2,
					 const uint8_t* src3, const uint8_t* src4, const uint8_t* src5, const size_t n )
{
	uint32_t v;

	v = 0xff000000;
	v |= BAYER_MIX2( HQPTR(src2,0), HQPTR(src4,0) ); // RED
	v |= *src3 << 8; // GREEN
	v |= *(src3+1) << 16; // BLUE
	*dst++ = v;
	src1++;
	src2++;
	src3++;
	src4++;
	src5++;

	v = 0xff000000;
	v |= BAYER_MIX4( HQPTR(src2,-1), HQPTR(src4,-1), HQPTR(src2,1), HQPTR(src4,1) ); // RED
	v |= BAYER_MIX4( HQPTR(src2,0), HQPTR(src4,0), HQPTR(src3,-1), HQPTR(src3,1) ) << 8; // GREEN
	v |= *src3 << 16; // BLUE
	*dst++ = v;
	src1++;
	src2++;
	src3++;
	src4++;
	src5++;

	size_t i = ( n - 4 ) >> 1;
	while( i-- ) {
		v = 0xff000000;
		v |= Math::clamp( BAYER_HQ_R_AT_G_ODD( src1, src2, src3, src4, src5 ), 0x0, 0xff ); // RED
		v |= *src3 << 8; // GREEN
		v |= Math::clamp( BAYER_HQ_B_AT_G_ODD( src1, src2, src3, src4, src5 ), 0x0, 0xff ) << 16; // BLUE
		*dst++ = v;
		src1++;
		src2++;
		src3++;
		src4++;
		src5++;

		v = 0xff000000;
		v |= Math::clamp( BAYER_HQ_R_AT_B_ODD( src1, src2, src3, src4, src5 ), 0x0, 0xff ); // RED
		v |= Math::clamp( BAYER_HQ_G_AT_B( src1, src2, src3, src4, src5 ), 0x0, 0xff ) << 8; // GREEN
		v |= *src3 << 16; // BLUE
		*dst++ = v;
		src1++;
		src2++;
		src3++;
		src4++;
		src5++;
	}

	v = 0xff000000;
	v |= BAYER_MIX2( HQPTR(src2,0), HQPTR(src4,0) ); // RED
	v |= *src3 << 8; // GREEN
	v |= BAYER_MIX2( HQPTR(src3,-1), HQPTR(src3,1) ) << 16; // BLUE
	*dst++ = v;
	src1++;
	src2++;
	src3++;
	src4++;
	src5++;

	v = 0xff000000;
	v |= BAYER_MIX2( HQPTR(src2,-1), HQPTR(src4,-1) ); // RED
	v |= BAYER_MIX4( HQPTR(src2,0), HQPTR(src4,0), HQPTR(src3,-1), HQPTR(src3,-1) ) << 8; // GREEN
	v |= *src3 << 16; // BLUE
	*dst++ = v;
	src1++;
	src2++;
	src3++;
	src4++;
	src5++;
}


void dodebayer( Image& out, const Image& bayer )
{
	out.reallocate( bayer.width(), bayer.height(), IFormat::RGBA_UINT8 );
	IMapScoped<uint32_t> dst( out );
	IMapScoped<const uint8_t> src( bayer );
	size_t sstride = src.stride();
	const uint8_t* s1 = src.ptr(); src++;
	const uint8_t* s2 = src.ptr(); src++;
	const uint8_t* s3 = src.ptr(); src++;
	const uint8_t* s4 = src.ptr(); src++;
	const uint8_t* s5 = src.ptr();
	size_t w = bayer.width();
	size_t h = bayer.height();

	dst.setLine( 2 );
	h = ( h >> 1 ) - 2;

	while( h-- ) {
		debayerhq_even( dst.ptr(), s1, s2, s3, s4, s5, w );
		s1 += sstride;
		s2 += sstride;
		s3 += sstride;
		s4 += sstride;
		s5 += sstride;
		dst++;

		debayerhq_odd( dst.ptr(), s1, s2, s3, s4, s5, w );
		s1 += sstride;
		s2 += sstride;
		s3 += sstride;
		s4 += sstride;
		s5 += sstride;
		dst++;
	}
}

int main()
{
	Image in, bayer, out;

	in.load( "../data/bbc-hd.png" );
	mosaic_image( bayer, in );

//	out.reallocate( in.width(), in.height(), IFormat::RGBA_UINT8 );
//	bayer.convert( out );
	dodebayer( out, bayer );
	out.save( "debayerhq.png" );

	bayer.convert( out, ICONVERT_DEBAYER_LINEAR );
	out.save( "debayer.png" );
	return 0;

	/*
		G B
		R G
		G B
	 */
	const uint8_t data1[] = { 0x01, 0xB1, 0x02, 0xB1, 0x03, 0xB1, 0x04, 0xB1, 0x01, 0xB1, 0x01, 0xB1, 0x01, 0xB1, 0x01, 0xB1 };
	const uint8_t data2[] = { 0xA1, 0x02, 0xA2, 0x02, 0xA3, 0x02, 0xA4, 0x02, 0xA1, 0x02, 0xA1, 0x02, 0xA1, 0x02, 0xA1, 0x02 };
	const uint8_t data3[] = { 0x04, 0xB2, 0x03, 0xB2, 0x02, 0xB2, 0x01, 0xB2, 0x03, 0xB2, 0x03, 0xB2, 0x03, 0xB2, 0x03, 0xB2 };

	uint8_t bout[ 16 ];
	uint8_t sseout[ 16 ];

	SIMD* simd = SIMD::instance();
	simd->debayer_ODD_RGGBu8_GRAYu8( ( uint32_t* ) bout,( uint32_t* ) data1,( uint32_t* ) data2,( uint32_t* ) data3, 4 );

	for( int i = 0; i < 16; i++ ) {
		printf("0x%02X ", bout[ i ] );
	}
	printf("\n");


	sse2_debayer_EVEN_RGGBu8_GRAYu8( ( uint32_t* ) sseout,( uint32_t* ) data1,( uint32_t* ) data2,( uint32_t* ) data3, 16 );
	for( int i = 0; i < 16; i++ ) {
		printf("0x%02X ", sseout[ i ] );
	}
	printf("\n");


	return 0;

	const __m128i maskODD = _mm_set1_epi16( 0xff00 );
	const __m128i maskEVEN = _mm_set1_epi16( 0xff );

	__m128i b1, b2, g1, g2, g3, r1, t, t2;

	t = _mm_loadu_si128( ( __m128i * ) data1 );
	g1 = _mm_and_si128( t, maskEVEN );
	b1 = _mm_srli_si128( _mm_and_si128( t, maskODD ), 1 );

	t = _mm_loadu_si128( ( __m128i * ) data2 );
	r1 = _mm_and_si128( t, maskEVEN );
	g2 = _mm_srli_si128( _mm_and_si128( t, maskODD ), 1 );

	t = _mm_loadu_si128( ( __m128i * ) data3 );
	g3 = _mm_and_si128( t, maskEVEN );
	b2 = _mm_srli_si128( _mm_and_si128( t, maskODD ), 1 );

	SIMD_print_u16hex( g1 );
	SIMD_print_u16hex( b1 );

	SIMD_print_u16hex( r1 );
	SIMD_print_u16hex( g2 );

	SIMD_print_u16hex( g3 );
	SIMD_print_u16hex( b2 );


	b1 = _mm_add_epi16( b1, b2 );
	g1 = _mm_add_epi16( g1, g3 );


	SIMD_print_u16hex( b1 );
	SIMD_print_u16hex( g1 );
	SIMD_print_u16hex( _mm_add_epi16( r1, _mm_slli_si128( r1, 2 ) ) );
/*
			t = _mm_mullo_epi16( g2, glum );
			t = _mm_add_epi16( t, _mm_srli_epi16( _mm_mullo_epi16( b1, blum ), 1 ) );
			t = _mm_add_epi16( t, _mm_srli_epi16( _mm_mullo_epi16( _mm_add_epi16( r1, _mm_srli_si128( r1, 2 ) ), rlum ), 1 ) );

			t2 = _mm_mullo_epi16( r1, rlum );
			t2 = _mm_add_epi16( t2, _mm_srli_epi16( _mm_mullo_epi16( _mm_add_epi16( b1, _mm_srli_si128( b1, 2 ) ), blum ), 2 ) );
			t2 = _mm_add_epi16( t2, _mm_srli_epi16( _mm_mullo_epi16( _mm_add_epi16( g1, _mm_add_epi16( g2, _mm_srli_si128( g2, 2 ) ) ), glum ), 2 ) );*/


}
