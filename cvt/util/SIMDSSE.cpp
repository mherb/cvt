#include <cvt/util/SIMDSSE.h>
#include <xmmintrin.h>


namespace cvt {

#define SSE_AAOP1_FLOAT( name, sseop, cop ) \
void SIMDSSE::name( float* dst, const float* src1, const float* src2, const size_t n ) const \
{                                                                                            \
		size_t i = n >> 2;                                                                   \
		size_t i2 = i & 0x07;																 \
        __m128 d, s1, s2;                                                                    \
																						     \
		i >>= 3;                                                                             \
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
		size_t i2 = i & 0x07;																 \
        __m128 d, s1;														                 \
		const __m128 v = _mm_set1_ps( value );												 \
																						     \
		i >>= 3;                                                                             \
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

SSE_ACOP1_FLOAT( AddValue1f, _mm_add_ps, + )
SSE_ACOP1_FLOAT( SubValue1f, _mm_sub_ps, - )
SSE_ACOP1_FLOAT( MulValue1f, _mm_mul_ps, * )
SSE_ACOP1_FLOAT( DivValue1f, _mm_div_ps, / )


#define SSE_ACOP1_AOP2_FLOAT( name, sseop1, cop1, sseop2, cop2 ) \
	void SIMDSSE::name( float* dst, float const* src1, const float value, const size_t n ) const  \
	{                                                                                             \
        if( value == 0.0f )                                                                       \
            return;                                                                               \
                                                                                                  \
        size_t i = n >> 2;                                                                        \
		size_t i2 = i & 0x7;                                                                      \
        __m128 v = _mm_set1_ps( value );                                                          \
        __m128 d, s;                                                                              \
                                                                                                  \
		i = i >> 3;																				  \
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
            *dst = *dst cop2 *src1 cop1 value;                                                    \
            dst++; src1++;                                                                        \
        }                                                                                         \
	}                                                                                             \

SSE_ACOP1_AOP2_FLOAT( MulAddValue1f, _mm_mul_ps, *, _mm_add_ps, + )
SSE_ACOP1_AOP2_FLOAT( MulSubValue1f, _mm_mul_ps, *, _mm_sub_ps, - )

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


	void SIMDSSE::warpLinePerspectiveBilinear4f( float* dst, const float* _src, size_t srcStride, size_t srcWidth, size_t srcHeight, const float* point, const float* normal, const size_t n ) const
	{
		const uint8_t* src = ( const uint8_t* ) _src;
		float px, py, pz;
		size_t i = n;
		__m128 r, v1, v2, v3, v4, ax1, ax2, ay1, ay2, one;

		one = _mm_set1_ps( 1.0f );

		px = point[ 0 ];
		py = point[ 1 ];
		pz = point[ 2 ];

		while( i-- )
		{
			float fx, fy;

			fx = px / pz;
			fy = py / pz;

			ax1 = _mm_set1_ps( fx + 1 - ( float ) ( int )( fx + 1 ) );
			ax2 = _mm_sub_ps( one, ax1 );
			ay1 = _mm_set1_ps( fy + 1 - ( float ) ( int )( fy + 1 ) );
			ay2 = _mm_sub_ps( one, ay1 );

#define VAL( fx, fy ) ( ( fx ) >= 0 && ( fx ) < ( int ) srcWidth && ( fy ) >= 0 && ( fy ) < ( int ) srcHeight ) ? _mm_loadu_ps( ( float* ) ( src + srcStride * ( fy ) + sizeof( float ) * ( ( fx ) * 4 ) ) ) : _mm_loadu_ps( dst );

			int lx = -1 + ( int )( fx + 1 );
			int ly = -1 + ( int )( fy + 1 );

			v3 = VAL( lx, ly );
			v4 = VAL( lx + 1, ly );
			v1 = _mm_add_ps( _mm_mul_ps( v3, ax2 ), _mm_mul_ps( v4, ax1 ) );
			v3 = VAL( lx, ly + 1 );
			v4 = VAL( lx + 1, ly + 1 );
			v2 = _mm_add_ps( _mm_mul_ps( v3, ax2 ), _mm_mul_ps( v4, ax1 ) );
			r = _mm_add_ps( _mm_mul_ps( v1, ay2 ), _mm_mul_ps( v2, ay1 ) );
			_mm_storeu_ps( dst, r );
			dst += 4;

			px += normal[ 0 ];
			py += normal[ 1 ];
			pz += normal[ 2 ];
#undef VAL
		}

	}

	void SIMDSSE::Memcpy( uint8_t* dst, uint8_t const* src, const size_t n ) const
	{
		size_t n2 = n >> 5;
		__m128i tmp;

		if( ( ( size_t ) src | ( size_t ) dst ) & 0xf ) {
			while( n2-- ) {
				tmp = _mm_loadu_si128( ( __m128i* ) src );
				_mm_storeu_si128( ( __m128i* ) dst, tmp );
				src += 16;
				dst += 16;
				tmp = _mm_loadu_si128( ( __m128i* ) src );
				_mm_storeu_si128( ( __m128i* ) dst, tmp );
				src += 16;
				dst += 16;
			}
		} else {
			while( n2-- ) {
				tmp = _mm_load_si128( ( __m128i* ) src );
				_mm_stream_si128( ( __m128i* ) dst, tmp );
				src += 16;
				dst += 16;
				tmp = _mm_load_si128( ( __m128i* ) src );
				_mm_stream_si128( ( __m128i* ) dst, tmp );
				src += 16;
				dst += 16;
			}
		}
		n2 = n & 0x1f;
		while( n2-- )
			*dst++ = *src++;
	}
}
