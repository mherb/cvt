#include <cvt/util/SIMD.h>
#include <cvt/util/SIMDDebug.h>

#include <cvt/gfx/Image.h>

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

int main()
{
	Image in, bayer, out;

	in.load( "../data/lena.png" );
	mosaic_image( bayer, in );

	out.reallocate( in.width(), in.height(), IFormat::GRAY_UINT8 );
	bayer.convert( out );

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
