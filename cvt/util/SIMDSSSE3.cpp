/*
 * File:   SIMDSSSE3.cpp
 * Author: sebi
 *
 * Created on August 1, 2011, 8:48 AM
 */

#include "SIMDSSSE3.h"

#include <tmmintrin.h>
#include <xmmintrin.h>

namespace cvt {

    size_t SIMDSSSE3::hammingDistance(const uint8_t* src1, const uint8_t* src2, size_t n) const
	{
		size_t bitcount = 0;

		static const uint8_t __attribute__((aligned( 16 ))) LUT[ 16 ] = { 0, 1, 1, 2,
																	   	  1, 2, 2, 3,
																		  1, 2, 2, 3,
																		  2, 3, 3, 4 };

		__m128i s1, s2, xored, highNibbles, lowNibbles, zero, sum1, sum2, sum;

		const __m128i mask = _mm_set1_epi8( 0x0f );
		const __m128i lut  = _mm_loadu_si128( ( __m128i* )LUT );
        zero = _mm_set1_epi8( 0 );


		size_t n16 = n >> 4;
		size_t r   = n & 0xff;
			
        sum1 = zero;
        sum2 = zero;

        // calculate the number of 31 blocks we need to process
        // n = 16 * 31 * x + r -> x / 496 + 1
        size_t numBlocks = n / ( 16 * 31 ) + 1;

		size_t num;
        while( numBlocks-- ){
			num = 31;
            while( n16-- && num-- ){
                s1 = _mm_loadu_si128( ( __m128i* )src1 );
                s2 = _mm_loadu_si128( ( __m128i* )src2 );

                // xor:
                xored = _mm_xor_si128( s1, s2 );

                // get the high and low nibbles:
                lowNibbles = _mm_and_si128( xored, mask );
                highNibbles = _mm_and_si128( _mm_srli_epi64( xored, 4 ), mask );

                // access the LUT
                sum1 = _mm_add_epi8( sum1, _mm_shuffle_epi8( lut, lowNibbles ) );
                sum2 = _mm_add_epi8( sum2, _mm_shuffle_epi8( lut, highNibbles ) );

                src1 += 16;
                src2 += 16;
            }
            
			// each 31 loops, accumulate the result into bitcount to
            // avoid overflow
            sum = _mm_sad_epu8( sum1, zero );
            sum = _mm_add_epi64( sum, _mm_sad_epu8( sum2, zero ) );

            
			sum = _mm_add_epi64( _mm_srli_si128( sum, 8 ), sum );

            bitcount += ( ( uint64_t* )( &sum ) )[ 0 ];
			sum1 = zero;
			sum2 = zero;
        }


        if( r ){
            uint64_t a = 0, b = 0;
            uint64_t xored;

            Memcpy( ( uint8_t* )( &a ), src1, r );
			Memcpy( ( uint8_t* )( &b ), src2, r );

            xored = ( a^b );
            xored = ( ( xored & 0xAAAAAAAAAAAAAAAAll ) >> 1 ) + ( xored & 0x5555555555555555ll );
            xored = ( ( xored & 0xCCCCCCCCCCCCCCCCll ) >> 2 ) + ( xored & 0x3333333333333333ll );
            xored = ( ( xored & 0xF0F0F0F0F0F0F0F0ll ) >> 4 ) + ( xored & 0x0F0F0F0F0F0F0F0Fll );
            xored += xored >> 32;
            xored += xored >> 16;
            xored += xored >>  8;
            //  1 8-bit number
            bitcount += ( xored & 0xFF );
        }

		return bitcount;
	}

}

