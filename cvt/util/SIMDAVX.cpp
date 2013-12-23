/*
   The MIT License (MIT)

   Copyright (c) 2011 - 2013, Philipp Heise and Sebastian Klose

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include <cvt/util/SIMDAVX.h>
#include <immintrin.h>

namespace cvt
{
	float SIMDAVX::SAD( const float* src1, const float* src2, const size_t n ) const
	{
		size_t i = n >> 3;

        const __m256i absmask =  _mm256_set1_epi32( 0x7fffffff );
		__m256 a, b, absdiff, sum;


		sum = _mm256_setzero_ps( );
		if( ( ( size_t ) src1 | ( size_t ) src2 ) & 0x1f ) {
			while( i-- ) {
				a = _mm256_loadu_ps( src1 );
				b = _mm256_loadu_ps( src2 );
				absdiff = _mm256_and_ps( _mm256_sub_ps( a, b ), ( __m256 ) absmask );
				sum = _mm256_add_ps( sum, absdiff );
				src1 += 8; src2 += 8;
			}
		} else {
			while( i-- ) {
				a = _mm256_load_ps( src1 );
				b = _mm256_load_ps( src2 );
				absdiff = _mm256_and_ps( _mm256_sub_ps( a, b ),( __m256 ) absmask );
				sum = _mm256_add_ps( sum, absdiff );
				src1 += 8; src2 += 8;
			}
		}

		float sad = 0.0f;

        __m128 sum2 = _mm_add_ps( _mm256_castps256_ps128( sum ), _mm256_extractf128_ps( sum, 1 ) );
        sum2 = _mm_add_ps( sum2, _mm_movehl_ps( sum2, sum2 ) );
        sum2 = _mm_add_ps( sum2, _mm_shuffle_ps( sum2, sum2, _MM_SHUFFLE( 0, 0, 0, 1 ) ) );
        _mm_store_ss( &sad, sum2 );

		i = n & 0x7;
		while( i-- ) {
			sad += Math::abs( *src1++ - *src2++ );
		}

		// Zero upper half of AVX registers to avoid AVX-SSE transition penalties
		_mm256_zeroupper( );

		return sad;
	}

	float SIMDAVX::SSD( const float* src1, const float* src2, const size_t n ) const
	{
		size_t i = n >> 3;

		__m256 a, b, diff, sqr, sum;
		sum = _mm256_setzero_ps( );
		if( ( ( size_t ) src1 | ( size_t ) src2 ) & 0x1f ) {
			while( i-- ) {
				a = _mm256_loadu_ps( src1 );
				b = _mm256_loadu_ps( src2 );
				diff = _mm256_sub_ps( a, b );
				sqr = _mm256_mul_ps( diff, diff );
				sum = _mm256_add_ps( sum, sqr );
				src1 += 8; src2 += 8;
			}
		} else {
			while( i-- ) {
				a = _mm256_load_ps( src1 );
				b = _mm256_load_ps( src2 );
				diff = _mm256_sub_ps( a, b );
				sqr = _mm256_mul_ps( diff, diff );
				sum = _mm256_add_ps( sum, sqr );
				src1 += 8; src2 += 8;
			}
		}

		float ssd = 0.0f;

        __m128 sum2 = _mm_add_ps( _mm256_castps256_ps128( sum ), _mm256_extractf128_ps( sum, 1 ) );
        sum2 = _mm_add_ps( sum2, _mm_movehl_ps( sum2, sum2 ) );
        sum2 = _mm_add_ps( sum2, _mm_shuffle_ps( sum2, sum2, _MM_SHUFFLE( 0, 0, 0, 1 ) ) );
        _mm_store_ss( &ssd, sum2 );

		i = n & 0x7;
		while( i-- ) {
			ssd += Math::sqr( *src1++ - *src2++ );
		}

		// Zero upper half of AVX registers to avoid AVX-SSE transition penalties
		_mm256_zeroupper( );

		return ssd;
	}

    float SIMDAVX::NCC( const float* src1, const float* src2, const size_t n ) const
    {
        size_t i = n >> 3;

        const float* ptr1 = src1;
        const float* ptr2 = src2;
        float mean1 = 0.0f;
        float mean2 = 0.0f;
        
#define sse_reduce_sum( ptr, r, r2 ) \
        __m128 r2 = _mm_add_ps( _mm256_castps256_ps128( r ), _mm256_extractf128_ps( r, 1 ) ); \
        r2 = _mm_add_ps( r2, _mm_movehl_ps( r2, r2 ) ); \
        r2 = _mm_add_ps( r2, _mm_shuffle_ps( r2, r2, _MM_SHUFFLE( 0, 0, 0, 1 ) ) ); \
        _mm_store_ss( ptr, r2 );

        __m256 a, b, simdSum, simdMean1, simdMean2, simdDiff1, simdDiff2, simdVar1, simdVar2;

        simdSum = _mm256_setzero_ps( );
        simdMean1 = _mm256_setzero_ps( );
        simdMean2 = _mm256_setzero_ps( );
        simdVar1 = _mm256_setzero_ps( );
        simdVar2 = _mm256_setzero_ps( );

        if( ( ( size_t ) src1 | ( size_t ) src2 ) & 0x1f ) {
            while( i-- ) {
                simdMean1 = _mm256_add_ps( simdMean1, _mm256_loadu_ps( ptr1 ) );
                simdMean2 = _mm256_add_ps( simdMean2, _mm256_loadu_ps( ptr2 ) );
                ptr1 += 8; ptr2 += 8;
            }

            sse_reduce_sum( &mean1, simdMean1, simdMean12 );
            sse_reduce_sum( &mean2, simdMean2, simdMean22 );

            i = n & 0x7;
            while( i-- ) {
                mean1 += *ptr1++;
                mean2 += *ptr2++;
            }

            mean1 /= (float) n;
            mean2 /= (float) n;
            simdMean1 = _mm256_broadcast_ss( &mean1 );
            simdMean2 = _mm256_broadcast_ss( &mean2 );

            i = n >> 3;
            while( i-- ) {
                a = _mm256_loadu_ps( src1 );
                b = _mm256_loadu_ps( src2 );
                simdDiff1 = _mm256_sub_ps( a, simdMean1 );
                simdDiff2 = _mm256_sub_ps( b, simdMean2 );
                simdSum = _mm256_add_ps( simdSum, _mm256_mul_ps( simdDiff1, simdDiff2 ) );
                simdVar1 = _mm256_add_ps( simdVar1, _mm256_mul_ps( simdDiff1, simdDiff1 ) );
                simdVar2 = _mm256_add_ps( simdVar2, _mm256_mul_ps( simdDiff2, simdDiff2 ) );
                src1 += 8; src2 += 8;
            }
        } else {
            while( i-- ) {
                simdMean1 = _mm256_add_ps( simdMean1, _mm256_load_ps( ptr1 ) );
                simdMean2 = _mm256_add_ps( simdMean2, _mm256_load_ps( ptr2 ) );
                ptr1 += 8; ptr2 += 8;
            }

            sse_reduce_sum( &mean1, simdMean1, simdMean12 );
            sse_reduce_sum( &mean2, simdMean2, simdMean22 );

            i = n & 0x7;
            while( i-- ) {
                mean1 += *ptr1++;
                mean2 += *ptr2++;
            }

            mean1 /= (float) n;
            mean2 /= (float) n;
            simdMean1 = _mm256_broadcast_ss( &mean1 );
            simdMean2 = _mm256_broadcast_ss( &mean2 );

            i = n >> 3;
            while( i-- ) {
                a = _mm256_load_ps( src1 );
                b = _mm256_load_ps( src2 );
                simdDiff1 = _mm256_sub_ps( a, simdMean1 );
                simdDiff2 = _mm256_sub_ps( b, simdMean2 );
                simdSum = _mm256_add_ps( simdSum, _mm256_mul_ps( simdDiff1, simdDiff2 ) );
                simdVar1 = _mm256_add_ps( simdVar1, _mm256_mul_ps( simdDiff1, simdDiff1 ) );
                simdVar2 = _mm256_add_ps( simdVar2, _mm256_mul_ps( simdDiff2, simdDiff2 ) );
                src1 += 8; src2 += 8;
            }
        }

        float sum = 0.0f;
        float var1 = 0.0f;
        float var2 = 0.0f;

        sse_reduce_sum( &sum, simdSum, simdSum2 );
        sse_reduce_sum( &var1, simdVar1, simdVar12 );
        sse_reduce_sum( &var2, simdVar2, simdVar22 );

#undef sse_reduce_sum
        
        i = n & 0x7;
        while( i-- ) {
            float diff1 = ( *src1++ - mean1 );
            float diff2 = ( *src2++ - mean2 );
            sum += diff1 * diff2;
            var1 += Math::sqr( diff1 );
            var2 += Math::sqr( diff2 );
        }
        
        // Zero upper half of AVX registers to avoid AVX-SSE transition penalties
        _mm256_zeroupper( );
        
        return Math::invSqrt( var1 * var2 ) * sum;
    }

}
