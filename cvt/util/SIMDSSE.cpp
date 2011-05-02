#include <cvt/util/SIMDSSE.h>
#include <xmmintrin.h>


namespace cvt {

#define SSE_AAOP1_FLOAT( name, sseop, cop ) \
void SIMDSSE::name( float* dst, const float* src1, const float* src2, const size_t n ) const \
{                                                                                            \
		size_t i = n >> 2;                                                                   \
		size_t i2 = i & 0x0f;																 \
        __m128 d, s1, s2;                                                                    \
																						     \
		i >>= 4;                                                                             \
        if( ( ( size_t )src1 | ( size_t )src2 | ( size_t )dst ) & 0xf ){                     \
			while( i-- ) {                                                                   \
                s1 = _mm_loadu_ps( src1 );                                                   \
                s2 = _mm_loadu_ps( src2 );                                                   \
                d = sseop( s1, s2 );														 \
                _mm_storeu_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
                s1 = _mm_loadu_ps( src1 );                                                   \
                s2 = _mm_loadu_ps( src2 );                                                   \
                d = sseop( s1, s2 );														 \
                _mm_storeu_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
                s1 = _mm_loadu_ps( src1 );                                                   \
                s2 = _mm_loadu_ps( src2 );                                                   \
                d = sseop( s1, s2 );														 \
                _mm_storeu_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
                s1 = _mm_loadu_ps( src1 );                                                   \
                s2 = _mm_loadu_ps( src2 );                                                   \
                d = sseop( s1, s2 );														 \
                _mm_storeu_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
                s1 = _mm_loadu_ps( src1 );                                                   \
                s2 = _mm_loadu_ps( src2 );                                                   \
                d = sseop( s1, s2 );														 \
                _mm_storeu_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
                s1 = _mm_loadu_ps( src1 );                                                   \
                s2 = _mm_loadu_ps( src2 );                                                   \
                d = sseop( s1, s2 );														 \
                _mm_storeu_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
                s1 = _mm_loadu_ps( src1 );                                                   \
                s2 = _mm_loadu_ps( src2 );                                                   \
                d = sseop( s1, s2 );														 \
                _mm_storeu_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
																							 \
                s1 = _mm_loadu_ps( src1 );                                                   \
                s2 = _mm_loadu_ps( src2 );                                                   \
                d = sseop( s1, s2 );														 \
                _mm_storeu_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
            }                                                                                \
                                                                                             \
            while( i2-- ) {                                                                  \
                s1 = _mm_loadu_ps( src1 );                                                   \
                s2 = _mm_loadu_ps( src2 );                                                   \
                d = sseop( s1, s2 );														 \
                _mm_storeu_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
            }                                                                                \
        } else {                                                                             \
			  while( i-- ) {                                                                 \
                s1 = _mm_load_ps( src1 );                                                    \
                s2 = _mm_load_ps( src2 );                                                    \
                d = sseop( s1, s2 );														 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
                s1 = _mm_load_ps( src1 );                                                    \
                s2 = _mm_load_ps( src2 );                                                    \
                d = sseop( s1, s2 );														 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
                s1 = _mm_load_ps( src1 );                                                    \
                s2 = _mm_load_ps( src2 );                                                    \
                d = sseop( s1, s2 );														 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
                s1 = _mm_load_ps( src1 );                                                    \
                s2 = _mm_load_ps( src2 );                                                    \
                d = sseop( s1, s2 );														 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
                s1 = _mm_load_ps( src1 );                                                    \
                s2 = _mm_load_ps( src2 );                                                    \
                d = sseop( s1, s2 );														 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
                s1 = _mm_load_ps( src1 );                                                    \
                s2 = _mm_load_ps( src2 );                                                    \
                d = sseop( s1, s2 );														 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
                s1 = _mm_load_ps( src1 );                                                    \
                s2 = _mm_load_ps( src2 );                                                    \
                d = sseop( s1, s2 );														 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
                                                                                             \
                s1 = _mm_load_ps( src1 );                                                    \
                s2 = _mm_load_ps( src2 );                                                    \
                d = sseop( s1, s2 );														 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
            }                                                                                \
                                                                                             \
            while( i2-- ) {                                                                  \
                s1 = _mm_load_ps( src1 );                                                    \
                s2 = _mm_load_ps( src2 );                                                    \
                d = sseop( s1, s2 );														 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                src2 += 4;                                                                   \
            }                                                                                \
        }                                                                                    \
                                                                                             \
		i = n & 0x03;                                                                        \
		while( i-- )                                                                         \
			*dst++ = *src1++ cop *src2++;                                                    \
}


SSE_AAOP1_FLOAT( Add, _mm_add_ps, + )
SSE_AAOP1_FLOAT( Sub, _mm_sub_ps, - )
SSE_AAOP1_FLOAT( Mul, _mm_mul_ps, * )
SSE_AAOP1_FLOAT( Div, _mm_div_ps, / )

#define SSE_ACOP1_FLOAT( name, sseop, cop ) \
void SIMDSSE::name( float* dst, const float* src1, const float value, const size_t n ) const \
{                                                                                            \
		size_t i = n >> 2;                                                                   \
		size_t i2 = i & 0x0f;																 \
        __m128 d, s1;														                 \
		const __m128 v = _mm_set1_ps( value );												 \
																						     \
		i >>= 4;                                                                             \
        if( ( ( size_t )src1 | ( size_t )dst ) & 0xf ) {				                     \
			while( i-- ) {                                                                   \
				s1 = _mm_loadu_ps( src1 );                                                   \
				d = sseop( s1, v );															 \
				_mm_storeu_ps( dst, d );                                                     \
                                                                                             \
				dst += 4;                                                                    \
				src1 += 4;                                                                   \
				s1 = _mm_loadu_ps( src1 );                                                   \
				d = sseop( s1, v );															 \
				_mm_storeu_ps( dst, d );                                                     \
                                                                                             \
				dst += 4;                                                                    \
				src1 += 4;                                                                   \
				s1 = _mm_loadu_ps( src1 );                                                   \
				d = sseop( s1, v );															 \
				_mm_storeu_ps( dst, d );                                                     \
                                                                                             \
				dst += 4;                                                                    \
				src1 += 4;                                                                   \
				s1 = _mm_loadu_ps( src1 );                                                   \
				d = sseop( s1, v );															 \
				_mm_storeu_ps( dst, d );                                                     \
                                                                                             \
				dst += 4;                                                                    \
				src1 += 4;                                                                   \
				s1 = _mm_loadu_ps( src1 );                                                   \
				d = sseop( s1, v );															 \
				_mm_storeu_ps( dst, d );                                                     \
                                                                                             \
				dst += 4;                                                                    \
				src1 += 4;                                                                   \
				s1 = _mm_loadu_ps( src1 );                                                   \
				d = sseop( s1, v );															 \
				_mm_storeu_ps( dst, d );                                                     \
                                                                                             \
				dst += 4;                                                                    \
				src1 += 4;                                                                   \
				s1 = _mm_loadu_ps( src1 );                                                   \
				d = sseop( s1, v );															 \
				_mm_storeu_ps( dst, d );                                                     \
                                                                                             \
				dst += 4;                                                                    \
				src1 += 4;                                                                   \
				s1 = _mm_loadu_ps( src1 );                                                   \
				d = sseop( s1, v );															 \
				_mm_storeu_ps( dst, d );                                                     \
                                                                                             \
				dst += 4;                                                                    \
				src1 += 4;                                                                   \
			}                                                                                \
                                                                                             \
            while( i2-- ) {                                                                  \
                s1 = _mm_loadu_ps( src1 );                                                   \
                d = sseop( s1, v );															 \
                _mm_storeu_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
            }                                                                                \
        } else {                                                                             \
			  while( i-- ) {                                                                 \
                s1 = _mm_load_ps( src1 );                                                    \
                d = sseop( s1, v );															 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                s1 = _mm_load_ps( src1 );                                                    \
                d = sseop( s1, v );															 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                s1 = _mm_load_ps( src1 );                                                    \
                d = sseop( s1, v );															 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                s1 = _mm_load_ps( src1 );                                                    \
                d = sseop( s1, v );															 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                s1 = _mm_load_ps( src1 );                                                    \
                d = sseop( s1, v );															 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                s1 = _mm_load_ps( src1 );                                                    \
                d = sseop( s1, v );															 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                s1 = _mm_load_ps( src1 );                                                    \
                d = sseop( s1, v );															 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
                s1 = _mm_load_ps( src1 );                                                    \
                d = sseop( s1, v );															 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
            }                                                                                \
                                                                                             \
            while( i2-- ) {                                                                  \
                s1 = _mm_load_ps( src1 );                                                    \
                d = sseop( s1, v );															 \
                _mm_stream_ps( dst, d );                                                     \
                                                                                             \
                dst += 4;                                                                    \
                src1 += 4;                                                                   \
            }                                                                                \
        }                                                                                    \
                                                                                             \
		i = n & 0x03;                                                                        \
		while( i-- )                                                                         \
			*dst++ = *src1++ cop value;	                                                     \
}

SSE_ACOP1_FLOAT( Add, _mm_add_ps, + )
SSE_ACOP1_FLOAT( Sub, _mm_sub_ps, - )
SSE_ACOP1_FLOAT( Mul, _mm_mul_ps, * )
SSE_ACOP1_FLOAT( Div, _mm_div_ps, / )


#define SSE_ACOP1_AOP2_FLOAT( name, sseop1, cop1, sseop2, cop2 ) \
	void SIMDSSE::name( float* dst, float const* src1, const float value, const size_t n ) const  \
	{                                                                                             \
        if( value == 0.0f )                                                                       \
            return;                                                                               \
                                                                                                  \
        size_t i = n >> 2;                                                                        \
		size_t i2 = i & 0xf;                                                                      \
        __m128 v = _mm_set1_ps( value );                                                          \
        __m128 d, s;                                                                              \
                                                                                                  \
        if( ( ( size_t )src1 | ( size_t )dst ) & 0xf ){                                           \
			while( i-- ){                                                                         \
				d = _mm_loadu_ps( dst );                                                          \
				s = _mm_loadu_ps( src1 );                                                         \
				s = sseop1( s, v );                                                               \
				d = sseop2( d, s );                                                               \
                                                                                                  \
				_mm_storeu_ps( dst, d );                                                          \
                                                                                                  \
				dst  += 4;                                                                        \
				src1 += 4;                                                                        \
				d = _mm_loadu_ps( dst );                                                          \
				s = _mm_loadu_ps( src1 );                                                         \
				s = sseop1( s, v );                                                               \
				d = sseop2( d, s );                                                               \
                                                                                                  \
				_mm_storeu_ps( dst, d );                                                          \
                                                                                                  \
				dst  += 4;                                                                        \
				src1 += 4;                                                                        \
				d = _mm_loadu_ps( dst );                                                          \
				s = _mm_loadu_ps( src1 );                                                         \
				s = sseop1( s, v );                                                               \
				d = sseop2( d, s );                                                               \
                                                                                                  \
				_mm_storeu_ps( dst, d );                                                          \
                                                                                                  \
				dst  += 4;                                                                        \
				src1 += 4;                                                                        \
				d = _mm_loadu_ps( dst );                                                          \
				s = _mm_loadu_ps( src1 );                                                         \
				s = sseop1( s, v );                                                               \
				d = sseop2( d, s );                                                               \
                                                                                                  \
				_mm_storeu_ps( dst, d );                                                          \
                                                                                                  \
				dst  += 4;                                                                        \
				src1 += 4;                                                                        \
				d = _mm_loadu_ps( dst );                                                          \
				s = _mm_loadu_ps( src1 );                                                         \
				s = sseop1( s, v );                                                               \
				d = sseop2( d, s );                                                               \
                                                                                                  \
				_mm_storeu_ps( dst, d );                                                          \
                                                                                                  \
				dst  += 4;                                                                        \
				src1 += 4;                                                                        \
				d = _mm_loadu_ps( dst );                                                          \
				s = _mm_loadu_ps( src1 );                                                         \
				s = sseop1( s, v );                                                               \
				d = sseop2( d, s );                                                               \
                                                                                                  \
				_mm_storeu_ps( dst, d );                                                          \
                                                                                                  \
				dst  += 4;                                                                        \
				src1 += 4;                                                                        \
				d = _mm_loadu_ps( dst );                                                          \
				s = _mm_loadu_ps( src1 );                                                         \
				s = sseop1( s, v );                                                               \
				d = sseop2( d, s );                                                               \
                                                                                                  \
				_mm_storeu_ps( dst, d );                                                          \
                                                                                                  \
				dst  += 4;                                                                        \
				src1 += 4;                                                                        \
                                                                                                  \
				d = _mm_loadu_ps( dst );                                                          \
				s = _mm_loadu_ps( src1 );                                                         \
				s = sseop1( s, v );                                                               \
				d = sseop2( d, s );                                                               \
                                                                                                  \
				_mm_storeu_ps( dst, d );                                                          \
                                                                                                  \
				dst  += 4;                                                                        \
				src1 += 4;                                                                        \
			}                                                                                     \
                                                                                                  \
            while( i2-- ){                                                                        \
                d = _mm_loadu_ps( dst );                                                          \
                s = _mm_loadu_ps( src1 );                                                         \
                s = sseop1( s, v );                                                               \
                d = sseop2( d, s );                                                               \
                                                                                                  \
                _mm_storeu_ps( dst, d );                                                          \
                                                                                                  \
                dst  += 4;                                                                        \
                src1 += 4;                                                                        \
            }                                                                                     \
        } else {                                                                                  \
			i = i >> 4;                                                                           \
            while( i-- ){                                                                         \
                d = _mm_load_ps( dst );                                                           \
                s = _mm_load_ps( src1 );                                                          \
                s = sseop1( s, v );                                                               \
                d = sseop2( d, s );                                                               \
                                                                                                  \
                _mm_store_ps( dst, d );                                                           \
                                                                                                  \
                dst  += 4;                                                                        \
                src1 += 4;                                                                        \
                d = _mm_load_ps( dst );                                                           \
                s = _mm_load_ps( src1 );                                                          \
                s = sseop1( s, v );                                                               \
                d = sseop2( d, s );                                                               \
                                                                                                  \
                _mm_store_ps( dst, d );                                                           \
                                                                                                  \
                dst  += 4;                                                                        \
                src1 += 4;                                                                        \
                d = _mm_load_ps( dst );                                                           \
                s = _mm_load_ps( src1 );                                                          \
                s = sseop1( s, v );                                                               \
                d = sseop2( d, s );                                                               \
                                                                                                  \
                _mm_store_ps( dst, d );                                                           \
                                                                                                  \
                dst  += 4;                                                                        \
                src1 += 4;                                                                        \
                                                                                                  \
                d = _mm_load_ps( dst );                                                           \
                s = _mm_load_ps( src1 );                                                          \
                s = sseop1( s, v );                                                               \
                d = sseop2( d, s );                                                               \
                                                                                                  \
                _mm_store_ps( dst, d );                                                           \
                                                                                                  \
                dst  += 4;                                                                        \
                src1 += 4;                                                                        \
                d = _mm_load_ps( dst );                                                           \
                s = _mm_load_ps( src1 );                                                          \
                s = sseop1( s, v );                                                               \
                d = sseop2( d, s );                                                               \
                                                                                                  \
                _mm_store_ps( dst, d );                                                           \
                                                                                                  \
                dst  += 4;                                                                        \
                src1 += 4;                                                                        \
                d = _mm_load_ps( dst );                                                           \
                s = _mm_load_ps( src1 );                                                          \
                s = sseop1( s, v );                                                               \
                d = sseop2( d, s );                                                               \
                                                                                                  \
                _mm_store_ps( dst, d );                                                           \
                                                                                                  \
                dst  += 4;                                                                        \
                src1 += 4;                                                                        \
                d = _mm_load_ps( dst );                                                           \
                s = _mm_load_ps( src1 );                                                          \
                s = sseop1( s, v );                                                               \
                d = sseop2( d, s );                                                               \
                                                                                                  \
                _mm_store_ps( dst, d );                                                           \
                                                                                                  \
                dst  += 4;                                                                        \
                src1 += 4;                                                                        \
                                                                                                  \
                d = _mm_load_ps( dst );                                                           \
                s = _mm_load_ps( src1 );                                                          \
                s = sseop1( s, v );                                                               \
                d = sseop2( d, s );                                                               \
                                                                                                  \
                _mm_store_ps( dst, d );                                                           \
                                                                                                  \
                dst  += 4;                                                                        \
                src1 += 4;                                                                        \
            }                                                                                     \
			while( i2-- ){                                                                        \
                d = _mm_load_ps( dst );                                                           \
                s = _mm_load_ps( src1 );                                                          \
                s = sseop1( s, v );                                                               \
                d = sseop2( d, s );                                                               \
                                                                                                  \
                _mm_store_ps( dst, d );                                                           \
                                                                                                  \
                dst  += 4;                                                                        \
                src1 += 4;                                                                        \
            }                                                                                     \
                                                                                                  \
        }                                                                                         \
                                                                                                  \
        i = n & 0x03;                                                                             \
        while( i-- ){                                                                             \
            *dst = *dst cop2 *src1 cop1 value;                                                   \
            dst++; src1++;                                                                        \
        }                                                                                         \
	}                                                                                             \

SSE_ACOP1_AOP2_FLOAT( MulAdd, _mm_mul_ps, *, _mm_add_ps, + )
SSE_ACOP1_AOP2_FLOAT( MulSub, _mm_mul_ps, *, _mm_sub_ps, - )

	void SIMDSSE::Conv_XYZAf_to_ZYXAf( float* dst, float const* src, const size_t n ) const
	{
		size_t i = n;
		__m128 x;

		if( ( ( size_t ) dst | ( size_t ) src ) & 0xF ) {
			while( i-- ) {
				x = _mm_loadu_ps( src );
				x = _mm_shuffle_ps( x, x, _MM_SHUFFLE( 3, 0, 1, 2 ) );
				_mm_storeu_ps( dst, x  );
				src += 4;
				dst += 4;
			}
		} else {
			while( i-- ) {
				x = _mm_load_ps( src );
				x = _mm_shuffle_ps( x, x, _MM_SHUFFLE( 3, 0, 1, 2 ) );
				_mm_stream_ps( dst, x  );
				src += 4;
				dst += 4;
			}
		}
	}
}
