#include <cvt/vision/FAST.h>
#include <xmmintrin.h>
#include <emmintrin.h>

namespace cvt {

    #define CHECK_BARRIER(lo, hi, other, flags)		\
    {                                               \
        __m128i diff = _mm_subs_epu8(lo, other);	\
        __m128i diff2 = _mm_subs_epu8(other, hi);	\
        __m128i z = _mm_setzero_si128();			\
        diff = _mm_cmpeq_epi8(diff, z);				\
        diff2 = _mm_cmpeq_epi8(diff2, z);			\
        flags = ~(_mm_movemask_epi8(diff) | (_mm_movemask_epi8(diff2) << 16)); \
    }

    void printBitMask( unsigned int mask )
    {
        for( size_t i = 0; i < 8*sizeof( int ); i++ ){
            if( mask & (1 << i) )
                std::cout << "1";
            else
                std::cout << "0";
        }
        std::cout << std::endl;
    }

    void FAST::detect9simd( const uint8_t* im, size_t stride, size_t width, size_t height, std::vector<Feature2Df> & features )
    {
        make_offsets( stride );
        const size_t tripleStride = 3 * stride;

        // The compiler refuses to reserve a register for this
        const __m128i barriers = _mm_set1_epi8( _threshold  );

        // xend is the beginning of the last pixels in the row that need to be processed in the normal way
        size_t xend = width - 3 - ( width - 3 ) % 16;

        im += tripleStride;
        const uint8_t * ptr;

        int upperBound, lowerBound;

        for ( size_t y = 3; y < height - 3; y++ ) {
            ptr = im + 3;
            for ( size_t x = 3; x < 16; x++ ){
                lowerBound = *ptr - _threshold;
                upperBound = *ptr + _threshold;
                if ( ( lowerBound > 0 && isDarkerCorner9( ptr, lowerBound ) ) ||
                     ( upperBound < 255 && isBrighterCorner9( ptr, upperBound ) ) ){
                    features.push_back( Feature2Df( x, y ) );
                }
                ptr++;
            }

            for ( size_t x = 16; x < xend; x += 16, ptr += 16 ) {
                __m128i lo, hi;
                {
                    const __m128i here = _mm_load_si128( (const __m128i*)ptr );

                    lo = _mm_subs_epu8( here, barriers );
                    hi = _mm_adds_epu8( barriers, here );
                }

                unsigned int ans_0, ans_8, possible;
                {
                    __m128i top = _mm_load_si128( ( const __m128i* )( ptr - tripleStride ) );
                    __m128i bottom = _mm_load_si128( ( const __m128i* )( ptr + tripleStride ) );

                    CHECK_BARRIER( lo, hi, top, ans_0 );
                    CHECK_BARRIER( lo, hi, bottom, ans_8 );

                    possible = ans_0 | ans_8;

                    if ( !possible ){
                        continue;
                    }
                }

                unsigned int ans_15, ans_1;
                {
                    __m128i a = _mm_loadu_si128( ( const __m128i* )( ptr - 1 - tripleStride ) );
                    __m128i c = _mm_insert_epi16( _mm_srli_si128( a, 2 ), *( const uint16_t* ) (ptr + 15 - tripleStride), 7 );
                    CHECK_BARRIER( lo, hi, a, ans_15 );
                    CHECK_BARRIER( lo, hi, c, ans_1 );
                    // 8 or (15 and 1 )
                    possible &= ans_8 | (ans_15 & ans_1);

                    if ( !possible )
                        continue;
                }

                unsigned int ans_9, ans_7;
                {
                    __m128i d = _mm_loadu_si128( ( const __m128i* )( ptr - 1 + tripleStride ) );
                    __m128i f = _mm_insert_epi16( _mm_srli_si128( d, 2 ), *( const uint16_t* )( ptr + 15 + tripleStride ), 7 );
                    CHECK_BARRIER( lo, hi, d, ans_9 );
                    CHECK_BARRIER( lo, hi, f, ans_7 );
                    possible &= ans_9 | ( ans_0 & ans_1 );
                    possible &= ans_7 | ( ans_15 & ans_0 );

                    if ( !possible )
                        continue;
                }

                unsigned int ans_12, ans_4;
                {
                    __m128i left = _mm_loadu_si128( ( const __m128i* )( ptr - 3 ) );
                    __m128i right = _mm_loadu_si128( ( const __m128i* )( ptr + 3 ) );
                    CHECK_BARRIER( lo, hi, left, ans_12 );
                    CHECK_BARRIER( lo, hi, right, ans_4 );
                    possible &= ans_12 | ( ans_4 & ( ans_1 | ans_7 ) );
                    possible &= ans_4 | ( ans_12 & ( ans_9 | ans_15 ) );

                    if ( !possible )
                        continue;
                }

                unsigned int ans_14, ans_6;
                {
                    __m128i ul = _mm_loadu_si128( ( const __m128i* ) ( ptr - 2 - 2 * stride ) );
                    __m128i lr = _mm_loadu_si128( ( const __m128i* ) ( ptr + 2 + 2 * stride ) );
                    CHECK_BARRIER( lo, hi, ul, ans_14 );
                    CHECK_BARRIER( lo, hi, lr, ans_6 );
                    {
                        const unsigned int ans_6_7 = ans_6 & ans_7;
                        possible &= ans_14 | (ans_6_7 & (ans_4 | (ans_8 & ans_9)));
                        possible &= ans_1 | (ans_6_7) | ans_12;
                    }
                    {
                        const unsigned int ans_14_15 = ans_14 & ans_15;
                        possible &= ans_6 | (ans_14_15 & (ans_12 | (ans_0 & ans_1)));
                        possible &= ans_9 | (ans_14_15) | ans_4;
                    }

                    if ( !possible )
                        continue;
                }

                unsigned int ans_10, ans_2;
                {
                    __m128i ll = _mm_loadu_si128( ( const __m128i* ) (ptr - 2 + 2 * stride) );
                    __m128i ur = _mm_loadu_si128( ( const __m128i* ) (ptr + 2 - 2 * stride) );
                    CHECK_BARRIER( lo, hi, ll, ans_10 );
                    CHECK_BARRIER( lo, hi, ur, ans_2 );
                    {
                        const unsigned int ans_1_2 = ans_1 & ans_2;
                        possible &= ans_10 | (ans_1_2 & ((ans_0 & ans_15) | ans_4));
                        possible &= ans_12 | (ans_1_2) | (ans_6 & ans_7);
                    }
                    {
                        const unsigned int ans_9_10 = ans_9 & ans_10;
                        possible &= ans_2 | (ans_9_10 & ((ans_7 & ans_8) | ans_12));
                        possible &= ans_4 | (ans_9_10) | (ans_14 & ans_15);
                    }
                    possible &= ans_8 | ans_14 | ans_2;
                    possible &= ans_0 | ans_10 | ans_6;

                    if ( !possible )
                        continue;
                }

                unsigned int ans_13, ans_5;
                {
                    __m128i g = _mm_loadu_si128( ( const __m128i* ) (ptr - 3 - stride ) );
                    __m128i l = _mm_loadu_si128( ( const __m128i* ) (ptr + 3 + stride ) );
                    CHECK_BARRIER( lo, hi, g, ans_13 );
                    CHECK_BARRIER( lo, hi, l, ans_5 );
                    const unsigned int ans_15_0 = ans_15 & ans_0;
                    const unsigned int ans_7_8 = ans_7 & ans_8;
                    {
                        const unsigned int ans_12_13 = ans_12 & ans_13;
                        possible &= ans_5 | (ans_12_13 & ans_14 & ((ans_15_0) | ans_10));
                        possible &= ans_7 | (ans_1 & ans_2) | (ans_12_13);
                        possible &= ans_2 | (ans_12_13) | (ans_7_8);
                    }
                    {
                        const unsigned int ans_4_5 = ans_4 & ans_5;
                        const unsigned int ans_9_10 = ans_9 & ans_10;
                        possible &= ans_13 | (ans_4_5 & ans_6 & ((ans_7_8) | ans_2));
                        possible &= ans_15 | (ans_4_5) | (ans_9_10);
                        possible &= ans_10 | (ans_4_5) | (ans_15_0);
                        possible &= ans_15 | (ans_9_10) | (ans_4_5);
                    }

                    possible &= ans_8 | (ans_13 & ans_14) | ans_2;
                    possible &= ans_0 | (ans_5 & ans_6) | ans_10;

                    if ( !possible )
                        continue;
                }


                unsigned int ans_11, ans_3;
                {
                    __m128i ii = _mm_loadu_si128( ( const __m128i* )( ptr - 3 + stride ) );
                    __m128i jj = _mm_loadu_si128( ( const __m128i* )( ptr + 3 - stride ) );
                    CHECK_BARRIER( lo, hi, ii, ans_11 );
                    CHECK_BARRIER( lo, hi, jj, ans_3 );
                    {
                        const int ans_2_3 = ans_2 & ans_3;
                        possible &= ans_11 | (ans_2_3 & ans_4 & ((ans_0 & ans_1) | (ans_5 & ans_6)));
                        possible &= ans_13 | (ans_7 & ans_8) | (ans_2_3);
                        possible &= ans_8 | (ans_2_3) | (ans_13 & ans_14);
                    }
                    {
                        const int ans_11_12 = ans_11 & ans_12;
                        possible &= ans_3 | (ans_10 & ans_11_12 & ((ans_8 & ans_9) | (ans_13 & ans_14)));
                        possible &= ans_1 | (ans_11_12) | (ans_6 & ans_7);
                        possible &= ans_6 | (ans_0 & ans_1) | (ans_11_12);
                    }
                    {
                        const int ans_3_4 = ans_3 & ans_4;
                        possible &= ans_9 | (ans_3_4) | (ans_14 & ans_15);
                        possible &= ans_14 | (ans_8 & ans_9) | (ans_3_4);
                    }
                    {
                        const int ans_10_11 = ans_10 & ans_11;
                        possible &= ans_5 | (ans_15 & ans_0) | (ans_10_11);
                        possible &= ans_0 | (ans_10_11) | (ans_5 & ans_6);
                    }

                    if ( !possible )
                        continue;

                }

                possible |= (possible >> 16);

                //if(possible & 0x0f) //Does this make it faster?
                {
                    if ( possible & (1 << 0) )
                        features.push_back( Feature2Df( x + 0, y ) );
                    if ( possible & (1 << 1) )
                        features.push_back( Feature2Df( x + 1, y ) );
                    if ( possible & (1 << 2) )
                        features.push_back( Feature2Df( x + 2, y ) );
                    if ( possible & (1 << 3) )
                        features.push_back( Feature2Df( x + 3, y ) );
                    if ( possible & (1 << 4) )
                        features.push_back( Feature2Df( x + 4, y ) );
                    if ( possible & (1 << 5) )
                        features.push_back( Feature2Df( x + 5, y ) );
                    if ( possible & (1 << 6) )
                        features.push_back( Feature2Df( x + 6, y ) );
                    if ( possible & (1 << 7) )
                        features.push_back( Feature2Df( x + 7, y ) );
                }

                //if(possible & 0xf0) //Does this mak( ,  fast)r?
                {
                    if ( possible & (1 << 8) )
                        features.push_back( Feature2Df( x + 8, y ) );
                    if ( possible & (1 << 9) )
                        features.push_back( Feature2Df( x + 9, y ) );
                    if ( possible & (1 << 10) )
                        features.push_back( Feature2Df( x + 10, y ) );
                    if ( possible & (1 << 11) )
                        features.push_back( Feature2Df( x + 11, y ) );
                    if ( possible & (1 << 12) )
                        features.push_back( Feature2Df( x + 12, y ) );
                    if ( possible & (1 << 13) )
                        features.push_back( Feature2Df( x + 13, y ) );
                    if ( possible & (1 << 14) )
                        features.push_back( Feature2Df( x + 14, y ) );
                    if ( possible & (1 << 15) )
                        features.push_back( Feature2Df( x + 15, y ) );
                }
            }

            for ( size_t x = xend; x < width - 3; x++ ){
                lowerBound = *ptr - _threshold;
                upperBound = *ptr + _threshold;
                if ( ( lowerBound > 0 && isDarkerCorner9( ptr, lowerBound ) ) ||
                     ( upperBound < 255 && isBrighterCorner9( ptr, upperBound ) ) ){
                    features.push_back( Feature2Df( x, y ) );
                }
                ptr++;
            }
            im += stride;
        }
    }
}
