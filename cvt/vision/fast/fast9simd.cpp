#include <cvt/vision/FAST.h>
#include <xmmintrin.h>
#include <emmintrin.h>

namespace cvt {

#define TEST_UPPER_LOWER_SIMD( val, result )                \
    brighter = _mm_subs_epu8( val, lower );                 \
    darker   = _mm_subs_epu8( upper, val );                 \
    brighter = _mm_cmpeq_epi8( brighter, zeros );           \
    darker   = _mm_cmpeq_epi8( darker, zeros );				\
    result   = ~( _mm_movemask_epi8( brighter ) | ( _mm_movemask_epi8( darker ) << 16 ) );

    void FAST::detect9simd( const uint8_t* im, size_t stride, size_t width, size_t height, std::vector<Feature2D> & features )
    {
        size_t h = height - 3;
        size_t w = width - 3;

        features.reserve( 1024 );
        int upperBound;
        int lowerBound;

        size_t tripleStride = stride * 3;
        im += tripleStride + 3; // first testable pixel

        // number of nonaligned pixel at the start of the row
        size_t numAligned = ( ( w - 16 ) >> 4 );
        size_t alignEnd =  16 + ( numAligned << 4 );

        const __m128i thresholds = _mm_set1_epi8( ( uint8_t )_threshold );
        const __m128i zeros = _mm_setzero_si128();
        __m128i upper, lower, v0, v1, brighter, darker;

        // intermediate result bit masks
        uint32_t p0, p1, p2, p3, p4, p5, p6, p7;
        uint32_t p8, p9, p10, p11, p12, p13, p14, p15;

        uint32_t possibleCorners;

        for( size_t y = 3; y < h; y++ ){
            const uint8_t * curr = im;

            for( size_t x = 3; x < 16; x++ ){
                lowerBound = *curr - _threshold;
                upperBound = *curr + _threshold;

                if( lowerBound > 0 && isDarkerCorner9( curr, lowerBound ) ){
                    features.push_back( Feature2D( x, y ) );
                } else {
                    if( upperBound < 255 && isBrighterCorner9( curr, upperBound ) ){
                        features.push_back( Feature2D( x, y ) );
                    }
                }
                curr++;
            }

            // now we can use aligned simd stuff
            for( size_t x = 16; x < alignEnd; x+=16 ){
				// load the next 16 values:
				{
					const __m128i vals = _mm_load_si128( (const __m128i *)curr );
					lower = _mm_subs_epu8( vals, thresholds );
					upper = _mm_adds_epu8( vals, thresholds );
				}

				// p0 | p8 ?
				v0 = _mm_load_si128( ( const __m128i* )( curr - tripleStride ) );
				v1 = _mm_load_si128( ( const __m128i* )( curr + tripleStride ) );

				// calc test results for pixel 0 and 8
				// pixel 0: p < lower -> p - lower < 0
				TEST_UPPER_LOWER_SIMD( v0, p0 );
				TEST_UPPER_LOWER_SIMD( v1, p8 );
				possibleCorners = ( p0 | p8 );
				if( !possibleCorners ) continue;

				v0 = _mm_loadu_si128( ( const __m128i* )( curr - 1 - tripleStride ) );
				v1 = _mm_insert_epi16( _mm_srli_si128( v0, 2 ), *( const unsigned short* )( curr + 15 - stride ), 7 );
				TEST_UPPER_LOWER_SIMD(v0, p15);
				TEST_UPPER_LOWER_SIMD(v1, p1);
				possibleCorners &= p8 | ( p15 & p1 );
				if ( ! possibleCorners ) continue;

				v0 = _mm_loadu_si128( (const __m128i* )( curr-1+tripleStride));
				v1 = _mm_insert_epi16( _mm_srli_si128( v0, 2 ), *( const unsigned short* )( curr+15+tripleStride), 7);
				TEST_UPPER_LOWER_SIMD(v0, p9);
				TEST_UPPER_LOWER_SIMD(v1, p7);
				possibleCorners &= p9 | (p0 & p1);
				possibleCorners &= p7 | (p15 & p0);
				if (!possibleCorners)
					continue;

				v0 = _mm_loadu_si128((const __m128i*)( curr - 3 ) );
				v1 = _mm_loadu_si128((const __m128i*)( curr + 3 ) );
				TEST_UPPER_LOWER_SIMD( v0, p12 );
				TEST_UPPER_LOWER_SIMD( v1, p4 );
				possibleCorners &= p12 | (p4 & (p1 | p7));
				possibleCorners &= p4 | (p12 & (p9 | p15));
				if (!possibleCorners) continue;

				v0 = _mm_loadu_si128( ( const __m128i* )( curr-2-2*stride ) );
				v1 = _mm_loadu_si128( ( const __m128i* )( curr+2+2*stride ) );
				TEST_UPPER_LOWER_SIMD( v0, p14 );
				TEST_UPPER_LOWER_SIMD( v1, p6 );
				{
					const unsigned int p6_7 = p6 & p7;
					possibleCorners &= p14 | (p6_7 & (p4 | (p8 & p9)));
					possibleCorners &= p1 | (p6_7) | p12;
				}
				{
					const unsigned int p14_15 = p14 & p15;
					possibleCorners &= p6 | (p14_15 & (p12 | (p0 & p1)));
					possibleCorners &= p9 | (p14_15) | p4;
				}
				if (!possibleCorners) continue;

				v0 = _mm_loadu_si128( ( const __m128i* )( curr-2+2*stride ) );
				v1 = _mm_loadu_si128( ( const __m128i* )( curr+2-2*stride ) );

				TEST_UPPER_LOWER_SIMD( v0, p10 );
				TEST_UPPER_LOWER_SIMD( v1, p2 );
				{
					const unsigned int p1_2 = p1 & p2;
					possibleCorners &= p10 | (p1_2 & ((p0 & p15) | p4));
					possibleCorners &= p12 | (p1_2) | (p6 & p7);
				}
				{
					const unsigned int p9_10 = p9 & p10;
					possibleCorners &= p2 | (p9_10 & ((p7 & p8) | p12));
					possibleCorners &= p4 | (p9_10) | (p14 & p15);
				}
				possibleCorners &= p8 | p14 | p2;
				possibleCorners &= p0 | p10 | p6;
				if (!possibleCorners) continue;

				v0 = _mm_loadu_si128( ( const __m128i* )( curr-3-stride ) );
				v1 = _mm_loadu_si128( ( const __m128i* )( curr+3+stride ) );
				TEST_UPPER_LOWER_SIMD( v0, p13 );
				TEST_UPPER_LOWER_SIMD( v1, p5 );

				const unsigned int p15_0 = p15 & p0;
				const unsigned int p7_8 = p7 & p8;
				{
					const unsigned int p12_13 = p12 & p13;
					possibleCorners &= p5 | (p12_13 & p14 & ((p15_0) | p10));
					possibleCorners &= p7 | (p1 & p2) | (p12_13);
					possibleCorners &= p2 | (p12_13) | (p7_8);
				}
				{
					const unsigned int p4_5 = p4 & p5;
					const unsigned int p9_10 = p9 & p10;
					possibleCorners &= p13 | (p4_5 & p6 & ((p7_8) | p2));
					possibleCorners &= p15 | (p4_5) | (p9_10);
					possibleCorners &= p10 | (p4_5) | (p15_0);
					possibleCorners &= p15 | (p9_10) | (p4_5);
				}

				possibleCorners &= p8 | (p13 & p14) | p2;
				possibleCorners &= p0 | (p5 & p6) | p10;
				if (!possibleCorners) continue;

				v0 = _mm_loadu_si128( ( const __m128i* )( curr-3+stride ) );
				v1 = _mm_loadu_si128( ( const __m128i* )( curr+3-stride ) );
				TEST_UPPER_LOWER_SIMD( v0, p11 );
				TEST_UPPER_LOWER_SIMD( v1, p3 );
				{
					const unsigned int p2_3 = p2 & p3;
					possibleCorners &= p11 | (p2_3 & p4 & ((p0 & p1) | (p5 & p6)));
					possibleCorners &= p13 | (p7 & p8) | (p2_3);
					possibleCorners &= p8 | (p2_3) | (p13 & p14);
				}
				{
					const unsigned int p11_12 = p11 & p12;
					possibleCorners &= p3 | (p10 & p11_12 & ((p8 & p9) | (p13 & p14)));
					possibleCorners &= p1 | (p11_12) | (p6 & p7);
					possibleCorners &= p6 | (p0 & p1) | (p11_12);
				}
				{
					const unsigned int p3_4 = p3 & p4;
					possibleCorners &= p9 | (p3_4) | (p14 & p15);
					possibleCorners &= p14 | (p8 & p9) | (p3_4);
				}
				{
					const unsigned int p10_11 = p10 & p11;
					possibleCorners &= p5 | (p15 & p0) | (p10_11);
					possibleCorners &= p0 | (p10_11) | (p5 & p6);
				}
				if (!possibleCorners) continue;

                // now evaluate:
                possibleCorners |= ( possibleCorners >> 16 );
                if( possibleCorners & ( 1 << 0 ) )
                    features.push_back( Feature2D( x + 0, y ) );
                if( possibleCorners & ( 1 << 1 ) )
                    features.push_back( Feature2D( x + 1, y ) );
                if( possibleCorners & ( 1 << 2 ) )
                    features.push_back( Feature2D( x + 2, y ) );
                if( possibleCorners & ( 1 << 3 ) )
                    features.push_back( Feature2D( x + 3, y ) );
                if( possibleCorners & ( 1 << 4 ) )
                    features.push_back( Feature2D( x + 4, y ) );
                if( possibleCorners & ( 1 << 5 ) )
                    features.push_back( Feature2D( x + 5, y ) );
                if( possibleCorners & ( 1<< 6 ) )
                    features.push_back( Feature2D( x + 6, y ) );
                if( possibleCorners & ( 1<< 7 ) )
                    features.push_back( Feature2D( x + 7, y ) );
                if( possibleCorners & ( 1<< 8 ) )
                    features.push_back( Feature2D( x + 8, y ) );
                if( possibleCorners & ( 1<< 9 ) )
                    features.push_back( Feature2D( x + 9, y ) );
                if( possibleCorners & ( 1<< 10 ) )
                    features.push_back( Feature2D( x + 10, y ) );
                if( possibleCorners & ( 1<< 11 ) )
                    features.push_back( Feature2D( x + 11, y ) );
                if( possibleCorners & ( 1<< 12 ) )
                    features.push_back( Feature2D( x + 12, y ) );
                if( possibleCorners & ( 1<< 13 ) )
                    features.push_back( Feature2D( x + 13, y ) );
                if( possibleCorners & ( 1<< 14 ) )
                    features.push_back( Feature2D( x + 14, y ) );
                if( possibleCorners & ( 1<< 15 ) )
                    features.push_back( Feature2D( x + 15, y ) );

                curr+=16;
			}

            // now do the rest
            for( size_t x = alignEnd; x < w; x++ ){
                lowerBound = *curr - _threshold;
                upperBound = *curr + _threshold;

                if( lowerBound > 0 && isDarkerCorner9( curr, lowerBound ) ){
                    features.push_back( Feature2D( x, y ) );
                } else {
                    if( upperBound < 255 && isBrighterCorner9( curr, upperBound ) ){
                        features.push_back( Feature2D( x, y ) );
                    }
                }
                curr++;
            }

            im += stride;
        }
    }

#undef TEST_UPPER_LOWER_SIMD

}
