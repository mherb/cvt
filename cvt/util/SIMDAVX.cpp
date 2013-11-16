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
	float SIMDAVX::SAD( float const* src1, float const* src2, const size_t n ) const
	{
		size_t i = n >> 3;

		__m256 a, b, diff, negdiff, max, sum;
		sum = _mm256_setzero_ps( );
		if( ( ( size_t ) src1 | ( size_t ) src2 ) & 0x1f ) {
			while( i-- ) {
				a = _mm256_loadu_ps( src1 );
				b = _mm256_loadu_ps( src2 );
				diff = _mm256_sub_ps( a, b );
				negdiff = _mm256_sub_ps( _mm256_setzero_ps( ), diff );
				max = _mm256_max_ps( diff, negdiff );
				sum = _mm256_add_ps( sum, max );
				src1 += 8; src2 += 8;
			}
		} else {
			while( i-- ) {
				a = _mm256_load_ps( src1 );
				b = _mm256_load_ps( src2 );
				diff = _mm256_sub_ps( a, b );
				negdiff = _mm256_sub_ps( _mm256_setzero_ps( ), diff );
				max = _mm256_max_ps( diff, negdiff );
				sum = _mm256_add_ps( sum, max );
				src1 += 8; src2 += 8;
			}
		}

		float sad = 0.0f;

		// Reduce sum to single float
		float tmp[ 8 ];
		if( ( ( size_t ) tmp ) & 0x1f ) {
			_mm256_storeu_ps( tmp, sum );
		} else {
			_mm256_stream_ps( tmp, sum );
		}

		i = 8;
		while( i-- ) {
			sad += tmp[ i ];
		}

		i = n & 0x7;
		while( i-- ) {
			sad += Math::abs( *src1++ - *src2++ );
		}

		// Zero upper half of AVX registers to avoid AVX-SSE transition penalties
		_mm256_zeroupper( );

		return sad;
	}

	float SIMDAVX::SSD( float const* src1, float const* src2, const size_t n ) const
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

		// Reduce sum to single float
		float tmp[ 8 ];
		if( ( ( size_t ) tmp ) & 0x1f ) {
			_mm256_storeu_ps( tmp, sum );
		} else {
			_mm256_stream_ps( tmp, sum );
		}

		i = 8;
		while( i-- ) {
			ssd += tmp[ i ];
		}

		i = n & 0x7;
		while( i-- ) {
			ssd += Math::sqr( *src1++ - *src2++ );
		}

		// Zero upper half of AVX registers to avoid AVX-SSE transition penalties
		_mm256_zeroupper( );

		return ssd;
	}

}
