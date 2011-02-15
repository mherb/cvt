#include <cvt/util/SIMDSSE2.h>

#include <xmmintrin.h>
#include <emmintrin.h>

namespace cvt
{
	static inline void prefetcht0( const void * p )
	{
		__asm__ volatile(   "prefetcht0 (%0)\n\t"
						 : : "r" (p)
						);
	}

	void SIMDSSE2::Mul( Fixed* dst, const Fixed* src, Fixed value, size_t n ) const
	{
		size_t i = n >> 2;

		__m128 mul = _mm_set1_ps( ( float ) value );
		__m128i in;
		__m128 inf;

		if( ( ( size_t ) src | ( size_t ) dst ) & 0xf ) {
			while( i-- ) {
				in = _mm_loadu_si128( ( __m128i* ) src );
				inf = _mm_mul_ps( mul, _mm_cvtepi32_ps( in ) );
				_mm_storeu_si128( ( __m128i* ) dst, _mm_cvtps_epi32( inf ) );
				src += 4;
				dst += 4;
			}
		} else {
			while( i-- ) {
				in = _mm_load_si128( ( __m128i* ) src );
				inf = _mm_mul_ps( mul, _mm_cvtepi32_ps( in ) );
				_mm_store_si128( ( __m128i* ) dst, _mm_cvtps_epi32( inf ) );
				src += 4;
				dst += 4;
			}
		}

		i = n & 0x03;
		while( i-- )
			*dst++ += *src++ * value;
	}

	void SIMDSSE2::MulAdd( Fixed* dst, const Fixed* src, Fixed value, size_t n ) const
	{
		size_t i = n >> 3;

		if( value.native() == 0 )
			return;

		const __m128 mul = _mm_set1_ps( ( float ) value );
		__m128i in, out, in2, out2;
		__m128 inf, inf2;

		if( ( ( size_t ) src | ( size_t ) dst ) & 0xf ) {
			while( i-- ) {
				in = _mm_loadu_si128( ( __m128i* ) src );
				out = _mm_loadu_si128( ( __m128i* ) dst );
				inf = _mm_mul_ps( _mm_cvtepi32_ps( in ), mul );
				out = _mm_add_epi32( out, _mm_cvtps_epi32( inf )  );
				_mm_storeu_si128( ( __m128i* ) dst, out );

				src += 4;
				dst += 4;

				in2 = _mm_loadu_si128( ( __m128i* ) ( src ) );
				out2 = _mm_loadu_si128( ( __m128i* ) ( dst ) );
				inf2 = _mm_mul_ps( _mm_cvtepi32_ps( in2 ), mul );
				out2 = _mm_add_epi32( out2, _mm_cvtps_epi32( inf2 )  );
				_mm_storeu_si128( ( __m128i* ) ( dst ), out2 );

				src += 4;
				dst += 4;
			}
		} else {
			while( i-- ) {
#if 1
				asm( "movdqa (%0), %%xmm1;\n\t"
					 "movdqa 16(%0), %%xmm2;\n\t"
					 "prefetcht0 32(%0);\n\t"
					 "cvtdq2ps %%xmm1, %%xmm1;\n\t"
					 "mulps %2, %%xmm1;\n\t"
					 "cvtps2dq %%xmm1, %%xmm1;\n\t"
					 "paddd  (%1), %%xmm1;\n\t"
					 "movdqa %%xmm1, (%1);\n\t"
					 "\n\t"
					 "cvtdq2ps %%xmm2, %%xmm2;\n\t"
					 "mulps %2, %%xmm2;\n\t"
					 "cvtps2dq %%xmm2, %%xmm2;\n\t"
					 "paddd  16(%1), %%xmm2;\n\t"
					 "movdqa %%xmm2, 16(%1);\n\t"
					 "\n\t"
					: /* no output */
					: "r"(src),"r"(dst), "x"(mul)
					: "xmm1","xmm2","xmm3","xmm4"
					);
#else
				in = _mm_load_si128( ( __m128i* ) ( src ) );
				in2 = _mm_load_si128( ( __m128i* ) ( src + 4 ) );

//				_mm_prefetch( ( __m128i* ) ( src + 8 ), _MM_HINT_T0 );

				out = _mm_load_si128( ( __m128i* ) ( dst ) );
				inf = _mm_cvtepi32_ps( in );
				inf = _mm_mul_ps( inf, mul );
				out = _mm_add_epi32( out, _mm_cvtps_epi32( inf )  );
				_mm_store_si128( ( __m128i* ) ( dst ), out );

				out2 = _mm_load_si128( ( __m128i* ) ( dst + 4 ) );
				inf2 = _mm_cvtepi32_ps( in2 );
				inf2 = _mm_mul_ps( inf2 , mul );
				out2 = _mm_add_epi32( out2, _mm_cvtps_epi32( inf2 )  );
				_mm_store_si128( ( __m128i* ) ( dst + 4 ), out2 );
#endif
				src += 8;
				dst += 8;
			}
		}

		i = n & 0x07;
		while( i-- )
			*dst++ += *src++ * value;
	}

	size_t SIMDSSE2::SAD( uint8_t const* src1, uint8_t const* src2, const size_t n ) const
	{
		size_t i = n >> 4;	
		size_t sad = 0;
				
		__m128i simdA, simdB, simdC;
		if( ( ( size_t ) src1 | ( size_t ) src2 ) & 0xf ) {
			while( i-- ) {
				simdA = _mm_loadu_si128( ( __m128i* )src1 );
				simdB = _mm_loadu_si128( ( __m128i* )src2 );			
				simdC = _mm_sad_epu8( simdA, simdB );
				sad += _mm_extract_epi16( simdC, 0 );
				sad += _mm_extract_epi16( simdC, 4 );				
				src1 += 16; src2 += 16;
			}
		} else {
			while( i-- ) {
				simdA = _mm_load_si128( ( __m128i* )src1 );
				simdB = _mm_load_si128( ( __m128i* )src2 );			
				simdC = _mm_sad_epu8( simdA, simdB );
				sad += _mm_extract_epi16( simdC, 0 );
				sad += _mm_extract_epi16( simdC, 4 );				
				src1 += 16; src2 += 16;
			}
		}
		
		i = n & 0x03;
		while( i-- ) {
			sad += Math::abs( ( int16_t )*src1++ - ( int16_t )*src2++ );
		}
		
		return sad;
	}

	void SIMDSSE2::ConvolveClampSet1fx( Fixed* dst, uint8_t const* src, const size_t width, const Fixed* weights, const size_t wn ) const
	{
		const Fixed* wp;
		const uint8_t* sp;
		Fixed tmp;
		size_t i, k, b1, b2;

		if( wn == 1 ) {
			MulAdd( dst, src, *weights, width );
			return;
		}

		b1 = ( wn - ( 1 - ( wn & 1 ) ) ) / 2;
		b2 = ( wn + ( 1 - ( wn & 1 ) ) ) / 2;

		/* border 1 */
		i = b1;
		while( i-- ) {
			wp = weights;
			sp = src;
			tmp = *sp * *wp++;
			k = i;
			while( k-- )
				tmp += *sp * *wp++;
			k = wn - 1 - i;
			while( k-- ) {
				tmp += *sp++ * *wp++;
			}
			*dst++ = tmp;
		}


		/* center */
		i = ( width - wn + 1 ) >> 4;
		while( i-- ) {
			__m128i f, z = _mm_setzero_si128(), s0 = z, s1 = z, s2 = z, s3 = z;
			__m128i x0, x1, x2, x3;
			k = wn;
			sp = src;
			wp = weights;

			while( k-- )
			{
				f = _mm_cvtsi32_si128( (*wp).native() );
				wp++;
				f = _mm_shuffle_epi32( f, 0 );
				f = _mm_packs_epi32( f, f );

				x0 = _mm_loadu_si128( (const __m128i*) sp );
				x2 = _mm_unpackhi_epi8( x0, z );
				x0 = _mm_unpacklo_epi8( x0, z );
				x1 = _mm_mulhi_epi16( x0, f );
				x3 = _mm_mulhi_epi16( x2, f );
				x0 = _mm_mullo_epi16( x0, f );
				x2 = _mm_mullo_epi16( x2, f );

				s0 = _mm_add_epi32( s0, _mm_unpacklo_epi16( x0, x1 ) );
				s1 = _mm_add_epi32( s1, _mm_unpackhi_epi16( x0, x1 ) );
				s2 = _mm_add_epi32( s2, _mm_unpacklo_epi16( x2, x3 ) );
				s3 = _mm_add_epi32( s3, _mm_unpackhi_epi16( x2, x3 ) );
				sp++;
			}
			_mm_storeu_si128((__m128i*)( dst ), s0);
			_mm_storeu_si128((__m128i*)( dst + 4 ), s1);
			_mm_storeu_si128((__m128i*)( dst + 8 ), s2);
			_mm_storeu_si128((__m128i*)( dst + 12 ), s3);
			dst += 16;
			src += 16;
		}

		i = ( width - wn + 1 ) & 0xf;
		while( i-- ) {
			k = wn;
			sp = src;
			wp = weights;
			tmp = *sp++ * *wp++;
			k--;
			while( k-- )
				tmp += *sp++ * *wp++;
			*dst++ = tmp;
			src++;
		}

		/* border 2 */
		i = b2;
		while( i-- ) {
			wp = weights;
			sp = src;
			tmp = *sp++ * *wp++;
			k = b1 + i;
			while( k-- ) {
				tmp += *sp++ * *wp++;
			}
			k = b2 - i;
			sp--;
			while( k-- )
				tmp += *sp * *wp++;
			*dst++ = tmp;
			src++;
		}
	}

	void SIMDSSE2::ConvolveClampAdd1fx( Fixed* dst, uint8_t const* src, const size_t width, const Fixed* weights, const size_t wn ) const
	{
		const Fixed* wp;
		const uint8_t* sp;
		Fixed tmp;
		size_t i, k, b1, b2;

		if( wn == 1 ) {
			MulAdd( dst, src, *weights, width );
			return;
		}

		b1 = ( wn - ( 1 - ( wn & 1 ) ) ) / 2;
		b2 = ( wn + ( 1 - ( wn & 1 ) ) ) / 2;

		/* border 1 */
		i = b1;
		while( i-- ) {
			wp = weights;
			sp = src;
			tmp = *sp * *wp++;
			k = i;
			while( k-- )
				tmp += *sp * *wp++;
			k = wn - 1 - i;
			while( k-- ) {
				tmp += *sp++ * *wp++;
			}
			*dst++ += tmp;
		}


		/* center */
		i = ( width - wn + 1 ) >> 4;
		while( i-- ) {
			__m128i f, z = _mm_setzero_si128(), s0 = z, s1 = z, s2 = z, s3 = z;
			__m128i x0, x1, x2, x3;
			k = wn;
			sp = src;
			wp = weights;

			while( k-- )
			{
				f = _mm_cvtsi32_si128( (*wp).native() );
				wp++;
				f = _mm_shuffle_epi32( f, 0 );
				f = _mm_packs_epi32( f, f );

				x0 = _mm_loadu_si128( (const __m128i*) sp );
				x2 = _mm_unpackhi_epi8( x0, z );
				x0 = _mm_unpacklo_epi8( x0, z );
				x1 = _mm_mulhi_epi16( x0, f );
				x3 = _mm_mulhi_epi16( x2, f );
				x0 = _mm_mullo_epi16( x0, f );
				x2 = _mm_mullo_epi16( x2, f );

				s0 = _mm_add_epi32( s0, _mm_unpacklo_epi16( x0, x1 ) );
				s1 = _mm_add_epi32( s1, _mm_unpackhi_epi16( x0, x1 ) );
				s2 = _mm_add_epi32( s2, _mm_unpacklo_epi16( x2, x3 ) );
				s3 = _mm_add_epi32( s3, _mm_unpackhi_epi16( x2, x3 ) );
				sp++;
			}

			x0 = _mm_loadu_si128( (__m128i*) dst );
			s0 = _mm_add_epi32( s0, x0 );
			_mm_storeu_si128((__m128i*)( dst ), s0);

			x1 = _mm_loadu_si128( (__m128i*) ( dst + 4 ) );
			s1 = _mm_add_epi32( s1, x1 );
			_mm_storeu_si128((__m128i*)( dst + 4 ), s1);

			x2 = _mm_loadu_si128( (__m128i*) ( dst + 8 ) );
			s2 = _mm_add_epi32( s2, x2 );
			_mm_storeu_si128((__m128i*)( dst + 8 ), s2);

			x3 = _mm_loadu_si128( (__m128i*) ( dst + 12 ) );
			s3 = _mm_add_epi32( s3, x3 );
			_mm_storeu_si128((__m128i*)( dst + 12 ), s3);

			dst += 16;
			src += 16;
		}

		i = ( width - wn + 1 ) & 0xf;
		while( i-- ) {
			k = wn;
			sp = src;
			wp = weights;
			tmp = *sp++ * *wp++;
			k--;
			while( k-- )
				tmp += *sp++ * *wp++;
			*dst++ += tmp;
			src++;
		}

		/* border 2 */
		i = b2;
		while( i-- ) {
			wp = weights;
			sp = src;
			tmp = *sp++ * *wp++;
			k = b1 + i;
			while( k-- ) {
				tmp += *sp++ * *wp++;
			}
			k = b2 - i;
			sp--;
			while( k-- )
				tmp += *sp * *wp++;
			*dst++ += tmp;
			src++;
		}
	}

	void SIMDSSE2::ConvolveClampSet4fx( Fixed* dst, uint8_t const* src, const size_t width, const Fixed* weights, const size_t wn ) const
	{
		const Fixed* wp;
		const uint8_t* sp;
		Fixed tmp[ 4 ], w;
		size_t i, k, b1, b2;

		if( wn == 1 ) {
			this->Mul( dst, src, *weights, width * 4 );
			return;
		}

		b1 = ( wn - ( 1 - ( wn & 1 ) ) ) / 2;
		b2 = ( wn + ( 1 - ( wn & 1 ) ) ) / 2;

		/* border 1 */
		i = b1;
		while( i-- ) {
			wp = weights;
			sp = src;
			tmp[ 0 ] = *( sp + 0 ) * *wp;
			tmp[ 1 ] = *( sp + 1 ) * *wp;
			tmp[ 2 ] = *( sp + 2 ) * *wp;
			tmp[ 3 ] = *( sp + 3 ) * *wp++;
			k = i;
			while( k-- ) {
				tmp[ 0 ] += *( sp + 0 ) * *wp;
				tmp[ 1 ] += *( sp + 1 ) * *wp;
				tmp[ 2 ] += *( sp + 2 ) * *wp;
				tmp[ 3 ] += *( sp + 3 ) * *wp++;
			}
			k = wn - 1 - i;
			while( k-- ) {
				tmp[ 0 ] += *( sp + 0 ) * *wp;
				tmp[ 1 ] += *( sp + 1 ) * *wp;
				tmp[ 2 ] += *( sp + 2 ) * *wp;
				tmp[ 3 ] += *( sp + 3 ) * *wp++;
				sp += 4;
			}
			*dst++ = tmp[ 0 ];
			*dst++ = tmp[ 1 ];
			*dst++ = tmp[ 2 ];
			*dst++ = tmp[ 3 ];
		}


		/* center */
		i = ( width - wn + 1 ) >> 2;
		while( i-- ) {
			__m128i f, z, s0, s1, s2, s3;
			__m128i x0, x1, x2, x3;
			k = wn;
			sp = src;
			wp = weights;

			z = s0 = s1 = s2 = s3 = _mm_setzero_si128();

			while( k-- )
			{
				f = _mm_cvtsi32_si128( (*wp).native() );
				wp++;
				f = _mm_shuffle_epi32( f, 0 );
				f = _mm_packs_epi32( f, f );

				x0 = _mm_loadu_si128( (const __m128i*) sp );
				x2 = _mm_unpackhi_epi8( x0, z );
				x0 = _mm_unpacklo_epi8( x0, z );
				x1 = _mm_mulhi_epi16( x0, f );
				x3 = _mm_mulhi_epi16( x2, f );
				x0 = _mm_mullo_epi16( x0, f );
				x2 = _mm_mullo_epi16( x2, f );

				s0 = _mm_add_epi32( s0, _mm_unpacklo_epi16( x0, x1 ) );
				s1 = _mm_add_epi32( s1, _mm_unpackhi_epi16( x0, x1 ) );
				s2 = _mm_add_epi32( s2, _mm_unpacklo_epi16( x2, x3 ) );
				s3 = _mm_add_epi32( s3, _mm_unpackhi_epi16( x2, x3 ) );
				sp += 4;
			}

			_mm_store_si128((__m128i*)( dst ), s0);
			_mm_store_si128((__m128i*)( dst + 4 ), s1);
			_mm_store_si128((__m128i*)( dst + 8 ), s2);
			_mm_store_si128((__m128i*)( dst + 12 ), s3);

			dst += 16;
			src += 16;
		}


		i = ( width - wn + 1 ) & 0x3;
		while( i-- ) {
			k = wn;
			sp = src;
			wp = weights;
			tmp[ 0 ] = *( sp + 0 ) * *wp;
			tmp[ 1 ] = *( sp + 1 ) * *wp;
			tmp[ 2 ] = *( sp + 2 ) * *wp;
			tmp[ 3 ] = *( sp + 3 ) * *wp++;
			sp += 4;
			k--;
			while( k-- ) {
				tmp[ 0 ] += *( sp + 0 ) * *wp;
				tmp[ 1 ] += *( sp + 1 ) * *wp;
				tmp[ 2 ] += *( sp + 2 ) * *wp;
				tmp[ 3 ] += *( sp + 3 ) * *wp++;
				sp += 4;
			}
			*dst++ = tmp[ 0 ];
			*dst++ = tmp[ 1 ];
			*dst++ = tmp[ 2 ];
			*dst++ = tmp[ 3 ];
			src += 4;
		}

		/* border 2 */
		i = b2;
		while( i-- ) {
			wp = weights;
			sp = src;
			tmp[ 0 ] = *( sp + 0 ) * *wp;
			tmp[ 1 ] = *( sp + 1 ) * *wp;
			tmp[ 2 ] = *( sp + 2 ) * *wp;
			tmp[ 3 ] = *( sp + 3 ) * *wp++;
			k = b1 + i;
			while( k-- ) {
				sp += 4;
				tmp[ 0 ] += *( sp + 0 ) * *wp;
				tmp[ 1 ] += *( sp + 1 ) * *wp;
				tmp[ 2 ] += *( sp + 2 ) * *wp;
				tmp[ 3 ] += *( sp + 3 ) * *wp++;
			}
			k = b2 - i;
			while( k-- ) {
				tmp[ 0 ] += *( sp + 0 ) * *wp;
				tmp[ 1 ] += *( sp + 1 ) * *wp;
				tmp[ 2 ] += *( sp + 2 ) * *wp;
				tmp[ 3 ] += *( sp + 3 ) * *wp++;
			}
			*dst++ = tmp[ 0 ];
			*dst++ = tmp[ 1 ];
			*dst++ = tmp[ 2 ];
			*dst++ = tmp[ 3 ];
			src += 4;
		}
	}

	void SIMDSSE2::ConvolveClampAdd4fx( Fixed* dst, uint8_t const* src, const size_t width, const Fixed* weights, const size_t wn ) const
	{
		const Fixed* wp;
		const uint8_t* sp;
		Fixed tmp[ 4 ], w;
		size_t i, k, b1, b2;

		if( wn == 1 ) {
			MulAdd( dst, src, *weights, width * 4 );
			return;
		}

		b1 = ( wn - ( 1 - ( wn & 1 ) ) ) / 2;
		b2 = ( wn + ( 1 - ( wn & 1 ) ) ) / 2;

		/* border 1 */
		i = b1;
		while( i-- ) {
			wp = weights;
			sp = src;
			tmp[ 0 ] = *( sp + 0 ) * *wp;
			tmp[ 1 ] = *( sp + 1 ) * *wp;
			tmp[ 2 ] = *( sp + 2 ) * *wp;
			tmp[ 3 ] = *( sp + 3 ) * *wp++;
			k = i;
			while( k-- ) {
				tmp[ 0 ] += *( sp + 0 ) * *wp;
				tmp[ 1 ] += *( sp + 1 ) * *wp;
				tmp[ 2 ] += *( sp + 2 ) * *wp;
				tmp[ 3 ] += *( sp + 3 ) * *wp++;
			}
			k = wn - 1 - i;
			while( k-- ) {
				tmp[ 0 ] += *( sp + 0 ) * *wp;
				tmp[ 1 ] += *( sp + 1 ) * *wp;
				tmp[ 2 ] += *( sp + 2 ) * *wp;
				tmp[ 3 ] += *( sp + 3 ) * *wp++;
				sp += 4;
			}
			*dst++ += tmp[ 0 ];
			*dst++ += tmp[ 1 ];
			*dst++ += tmp[ 2 ];
			*dst++ += tmp[ 3 ];
		}


		/* center */
		i = ( width - wn + 1 ) >> 2;
		while( i-- ) {
			__m128i f, z = _mm_setzero_si128(), s0 = z, s1 = z, s2 = z, s3 = z;
			__m128i x0, x1, x2, x3;
			k = wn;
			sp = src;
			wp = weights;

			while( k-- )
			{
				f = _mm_cvtsi32_si128( (*wp).native() );
				wp++;
				f = _mm_shuffle_epi32(f, 0);
				f = _mm_packs_epi32(f, f);

				x0 = _mm_loadu_si128( (const __m128i*) sp );
				x2 = _mm_unpackhi_epi8(x0, z);
				x0 = _mm_unpacklo_epi8(x0, z);
				x1 = _mm_mulhi_epi16(x0, f);
				x3 = _mm_mulhi_epi16(x2, f);
				x0 = _mm_mullo_epi16(x0, f);
				x2 = _mm_mullo_epi16(x2, f);

				s0 = _mm_add_epi32(s0, _mm_unpacklo_epi16(x0, x1));
				s1 = _mm_add_epi32(s1, _mm_unpackhi_epi16(x0, x1));
				s2 = _mm_add_epi32(s2, _mm_unpacklo_epi16(x2, x3));
				s3 = _mm_add_epi32(s3, _mm_unpackhi_epi16(x2, x3));
				sp += 4;
			}

			x0 = _mm_load_si128( (__m128i*) dst );
			s0 = _mm_add_epi32( s0, x0 );
			_mm_store_si128((__m128i*)( dst ), s0);

			x1 = _mm_load_si128( (__m128i*) ( dst + 4 ) );
			s1 = _mm_add_epi32( s1, x1 );
			_mm_store_si128((__m128i*)( dst + 4 ), s1);

			x2 = _mm_load_si128( (__m128i*) ( dst + 8 ) );
			s2 = _mm_add_epi32( s2, x2 );
			_mm_store_si128((__m128i*)( dst + 8 ), s2);

			x3 = _mm_load_si128( (__m128i*) ( dst + 12 ) );
			s3 = _mm_add_epi32( s3, x3 );
			_mm_store_si128((__m128i*)( dst + 12 ), s3);

			dst += 16;
			src += 16;
		}

		i = ( width - wn + 1 ) & 0x3;
		while( i-- ) {
			k = wn;
			sp = src;
			wp = weights;
			w = *wp++;
			tmp[ 0 ] = *( sp + 0 ) * w;
			tmp[ 1 ] = *( sp + 1 ) * w;
			tmp[ 2 ] = *( sp + 2 ) * w;
			tmp[ 3 ] = *( sp + 3 ) * w;
			sp += 4;
			k--;
			while( k-- ) {
				tmp[ 0 ] += *( sp + 0 ) * *wp;
				tmp[ 1 ] += *( sp + 1 ) * *wp;
				tmp[ 2 ] += *( sp + 2 ) * *wp;
				tmp[ 3 ] += *( sp + 3 ) * *wp++;
				sp += 4;
			}
			*dst++ += tmp[ 0 ];
			*dst++ += tmp[ 1 ];
			*dst++ += tmp[ 2 ];
			*dst++ += tmp[ 3 ];
			src += 4;
		}

		/* border 2 */
		i = b2;
		while( i-- ) {
			wp = weights;
			sp = src;
			tmp[ 0 ] = *( sp + 0 ) * *wp;
			tmp[ 1 ] = *( sp + 1 ) * *wp;
			tmp[ 2 ] = *( sp + 2 ) * *wp;
			tmp[ 3 ] = *( sp + 3 ) * *wp++;
			k = b1 + i;
			while( k-- ) {
				sp += 4;
				tmp[ 0 ] += *( sp + 0 ) * *wp;
				tmp[ 1 ] += *( sp + 1 ) * *wp;
				tmp[ 2 ] += *( sp + 2 ) * *wp;
				tmp[ 3 ] += *( sp + 3 ) * *wp++;
			}
			k = b2 - i;
			while( k-- ) {
				tmp[ 0 ] += *( sp + 0 ) * *wp;
				tmp[ 1 ] += *( sp + 1 ) * *wp;
				tmp[ 2 ] += *( sp + 2 ) * *wp;
				tmp[ 3 ] += *( sp + 3 ) * *wp++;
			}
			*dst++ += tmp[ 0 ];
			*dst++ += tmp[ 1 ];
			*dst++ += tmp[ 2 ];
			*dst++ += tmp[ 3 ];
			src += 4;
		}
	}

	void SIMDSSE2::Conv_fx_to_u8( uint8_t* dst, const Fixed* src, const size_t n ) const
	{
		size_t i = n >> 4;
		__m128i x0, x1, x2, x3, rnd;
		rnd = _mm_set1_epi32( 0x8000 );

		if( ( ( size_t ) src | ( size_t ) dst ) & 0xf ) {
			while( i-- ) {
				x0 = _mm_loadu_si128( ( __m128i* ) src );
				x1 = _mm_loadu_si128( ( __m128i* ) ( src + 4 ) );
				x2 = _mm_loadu_si128( ( __m128i* ) ( src + 8 ) );
				x3 = _mm_loadu_si128( ( __m128i* ) ( src + 12 ) );

				x0 = _mm_srai_epi32( _mm_add_epi32( x0, rnd ), 16 );
				x1 = _mm_srai_epi32( _mm_add_epi32( x1, rnd ), 16 );
				x2 = _mm_srai_epi32( _mm_add_epi32( x2, rnd ), 16 );
				x3 = _mm_srai_epi32( _mm_add_epi32( x3, rnd ), 16 );

				x0 = _mm_packs_epi32( x0, x1 );
				x1 = _mm_packs_epi32( x2, x3 );

				x0 = _mm_packus_epi16( x0, x1 );
				_mm_storeu_si128( ( __m128i* ) dst, x0 );
				src += 16;
				dst += 16;
			}
		} else {
			while( i-- ) {
				x0 = _mm_load_si128( ( __m128i* ) src );
				x1 = _mm_load_si128( ( __m128i* ) ( src + 4 ) );
				x2 = _mm_load_si128( ( __m128i* ) ( src + 8 ) );
				x3 = _mm_load_si128( ( __m128i* ) ( src + 12 ) );

				x0 = _mm_srai_epi32( _mm_add_epi32( x0, rnd ), 16 );
				x1 = _mm_srai_epi32( _mm_add_epi32( x1, rnd ), 16 );
				x2 = _mm_srai_epi32( _mm_add_epi32( x2, rnd ), 16 );
				x3 = _mm_srai_epi32( _mm_add_epi32( x3, rnd ), 16 );

				x0 = _mm_packs_epi32( x0, x1 );
				x1 = _mm_packs_epi32( x2, x3 );

				x0 = _mm_packus_epi16( x0, x1 );
				_mm_store_si128( ( __m128i* ) dst, x0 );
				src += 16;
				dst += 16;
			}

		}

		i = n & 0x0f;
		while( i-- ) {
			*dst++ = ( uint8_t ) Math::clamp( src->round(), 0x0, 0xff );
			src++;
		}
	}

	void SIMDSSE2::Conv_YUYVu8_to_RGBAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const
	{
		const __m128i Y2RGB = _mm_set_epi16( 1192, 0, 1192, 0, 1192, 0, 1192, 0 );
		const __m128i UV2R  = _mm_set_epi16( 1634, 0, 1634, 0, 1634, 0, 1634, 0 );
		const __m128i UV2G  = _mm_set_epi16( -832, -401, -832, -401, -832, -401, -832, -401 );
		const __m128i UV2B  = _mm_set_epi16( 0, 2066, 0, 2066, 0, 2066, 0, 2066 );
		const __m128i UVOFFSET = _mm_set1_epi16( 128 );
		const __m128i YOFFSET = _mm_set1_epi16( 16 );
		const __m128i A32  = _mm_set1_epi32( 0xff );
		const __m128i mask = _mm_set1_epi16( 0xff00 );

		__m128i uyvy;
		__m128i uv, yz, y, z;
		__m128i uvR, uvG, uvB;
		__m128i r, g, b, a;
		__m128i RB0, RB1, GA0, GA1;

		int i = n >> 3;
		if( ( ( size_t ) src | ( size_t ) dst ) & 0xf ) {
			while( i-- ) {
				uyvy = _mm_loadu_si128( ( __m128i* ) src );
				src += 16;
				/* U0 Y0 V0 Z0 U1 Y1 V1 Z1 U2 Y2 V2 Z2 U3 Y3 V3 Z3 */

				uv = _mm_and_si128( mask, uyvy );
				uv = _mm_srli_si128( uv, 1 );
				uv = _mm_sub_epi16( uv, UVOFFSET ); /* U0 V0 U1 V1 ... */

				yz = _mm_andnot_si128( mask, uyvy );
				yz = _mm_sub_epi16( yz, YOFFSET );  /* Y0 Z0 Y1 Z1 ...  */

				z = _mm_madd_epi16( yz, Y2RGB );                      /* Z0 Z1 Z2 Z3 */
				y = _mm_madd_epi16( yz, _mm_srli_si128( Y2RGB, 2 ) ); /* Y0 Y1 Y2 Y3 */

				uvR = _mm_madd_epi16( uv, UV2R );
				uvG = _mm_madd_epi16( uv, UV2G );
				uvB = _mm_madd_epi16( uv, UV2B );

				r  = _mm_srai_epi32( _mm_add_epi32( y, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( y, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( y, uvB ), 10 );

				RB0 = _mm_packs_epi32( r, b );
				GA0 = _mm_packs_epi32( g, A32 );

				r  = _mm_srai_epi32( _mm_add_epi32( z, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( z, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( z, uvB ), 10 );

				RB1 = _mm_packs_epi32( r, b );
				GA1 = _mm_packs_epi32( g, A32 );

				r  = _mm_unpacklo_epi16( RB0, RB1 );
				b  = _mm_unpackhi_epi16( RB0, RB1 );
				g  = _mm_unpacklo_epi16( GA0, GA1 );
				a  = _mm_unpackhi_epi16( GA0, GA1 );

				RB0 = _mm_unpacklo_epi16( r, b );
				RB1 = _mm_unpackhi_epi16( r, b );
				RB0 = _mm_packus_epi16( RB0, RB1 );

				GA0 = _mm_unpacklo_epi16( g, a );
				GA1 = _mm_unpackhi_epi16( g, a );
				GA0 = _mm_packus_epi16( GA0, GA1 );

				_mm_storeu_si128( ( __m128i* ) dst,  _mm_unpacklo_epi8( RB0, GA0 ) );
				dst += 16;
				_mm_storeu_si128( ( __m128i* ) dst,  _mm_unpackhi_epi8( RB0, GA0 ) );
				dst += 16;
			}
		} else {
			while( i-- ) {
				uyvy = _mm_load_si128( ( __m128i* ) src );
				src += 16;
				/* U0 Y0 V0 Z0 U1 Y1 V1 Z1 U2 Y2 V2 Z2 U3 Y3 V3 Z3 */

				uv = _mm_and_si128( mask, uyvy );
				uv = _mm_srli_si128( uv, 1 );
				uv = _mm_sub_epi16( uv, UVOFFSET ); /* U0 V0 U1 V1 ... */

				yz = _mm_andnot_si128( mask, uyvy );
				yz = _mm_sub_epi16( yz, YOFFSET );  /* Y0 Z0 Y1 Z1 ...  */

				z = _mm_madd_epi16( yz, Y2RGB );                      /* Z0 Z1 Z2 Z3 */
				y = _mm_madd_epi16( yz, _mm_srli_si128( Y2RGB, 2 ) ); /* Y0 Y1 Y2 Y3 */

				uvR = _mm_madd_epi16( uv, UV2R );
				uvG = _mm_madd_epi16( uv, UV2G );
				uvB = _mm_madd_epi16( uv, UV2B );

				r  = _mm_srai_epi32( _mm_add_epi32( y, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( y, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( y, uvB ), 10 );

				RB0 = _mm_packs_epi32( r, b );
				GA0 = _mm_packs_epi32( g, A32 );

				r  = _mm_srai_epi32( _mm_add_epi32( z, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( z, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( z, uvB ), 10 );

				RB1 = _mm_packs_epi32( r, b );
				GA1 = _mm_packs_epi32( g, A32 );

				r  = _mm_unpacklo_epi16( RB0, RB1 );
				b  = _mm_unpackhi_epi16( RB0, RB1 );
				g  = _mm_unpacklo_epi16( GA0, GA1 );
				a  = _mm_unpackhi_epi16( GA0, GA1 );

				RB0 = _mm_unpacklo_epi16( r, b );
				RB1 = _mm_unpackhi_epi16( r, b );
				RB0 = _mm_packus_epi16( RB0, RB1 );

				GA0 = _mm_unpacklo_epi16( g, a );
				GA1 = _mm_unpackhi_epi16( g, a );
				GA0 = _mm_packus_epi16( GA0, GA1 );

				_mm_stream_si128( ( __m128i* ) dst,  _mm_unpacklo_epi8( RB0, GA0 ) );
				dst += 16;
				_mm_stream_si128( ( __m128i* ) dst,  _mm_unpackhi_epi8( RB0, GA0 ) );
				dst += 16;
			}
		}

		uint32_t* dst32 = ( uint32_t* ) dst;
		uint32_t* src32 = ( uint32_t* ) src;
		i = ( n & 0x7 ) >> 1;
		while( i-- ) {
			uint32_t in, out;
			int r, g, b, y0, y1, u, v;

			in = *src32++;
			v = ( in >> 24 ) - 128;
			y1 = ( ( ( int ) ( ( in >> 16 ) & 0xff ) - 16 ) * 1192 ) >> 10;
			u = ( ( in >> 8 ) & 0xff ) - 128;
			y0 = ( ( ( int ) ( in & 0xff ) - 16 ) * 1192 ) >> 10;
			r = ((v*1634) >> 10);
			g = ((u*401 + v*832) >> 10);
			b = ((u*2066) >> 10);

			// clamp the values
			out = 0xff000000;
			out |= Math::clamp( y0 + r, 0, 255 );
			out |= Math::clamp( y0 - g, 0, 255 ) << 8;
			out |= Math::clamp( y0 + b, 0, 255 ) << 16;
			*dst32++ = out;
			out = 0xff000000;
			out |= Math::clamp( y1 + r, 0, 255 );
			out |= Math::clamp( y1 - g, 0, 255 ) << 8;
			out |= Math::clamp( y1 + b, 0, 255 ) << 16;
			*dst32++ = out;
		}

	}

	void SIMDSSE2::Conv_YUYVu8_to_BGRAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const
	{
		const __m128i Y2RGB = _mm_set_epi16( 1192, 0, 1192, 0, 1192, 0, 1192, 0 );
		const __m128i UV2R  = _mm_set_epi16( 1634, 0, 1634, 0, 1634, 0, 1634, 0 );
		const __m128i UV2G  = _mm_set_epi16( -832, -401, -832, -401, -832, -401, -832, -401 );
		const __m128i UV2B  = _mm_set_epi16( 0, 2066, 0, 2066, 0, 2066, 0, 2066 );
		const __m128i UVOFFSET = _mm_set1_epi16( 128 );
		const __m128i YOFFSET = _mm_set1_epi16( 16 );
		const __m128i A32  = _mm_set1_epi32( 0xff );
		const __m128i mask = _mm_set1_epi16( 0xff00 );

		__m128i uyvy;
		__m128i uv, yz, y, z;
		__m128i uvR, uvG, uvB;
		__m128i r, g, b, a;
		__m128i RB0, RB1, GA0, GA1;

		int i = n >> 3;
		if( ( ( size_t ) src | ( size_t ) dst ) & 0xf ) {
			while( i-- ) {
				uyvy = _mm_loadu_si128( ( __m128i* ) src );
				src += 16;
				/* U0 Y0 V0 Z0 U1 Y1 V1 Z1 U2 Y2 V2 Z2 U3 Y3 V3 Z3 */

				uv = _mm_and_si128( mask, uyvy );
				uv = _mm_srli_si128( uv, 1 );
				uv = _mm_sub_epi16( uv, UVOFFSET ); /* U0 V0 U1 V1 ... */

				yz = _mm_andnot_si128( mask, uyvy );
				yz = _mm_sub_epi16( yz, YOFFSET );  /* Y0 Z0 Y1 Z1 ...  */

				z = _mm_madd_epi16( yz, Y2RGB );                      /* Z0 Z1 Z2 Z3 */
				y = _mm_madd_epi16( yz, _mm_srli_si128( Y2RGB, 2 ) ); /* Y0 Y1 Y2 Y3 */

				uvR = _mm_madd_epi16( uv, UV2R );
				uvG = _mm_madd_epi16( uv, UV2G );
				uvB = _mm_madd_epi16( uv, UV2B );

				r  = _mm_srai_epi32( _mm_add_epi32( y, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( y, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( y, uvB ), 10 );

				RB0 = _mm_packs_epi32( r, b );
				GA0 = _mm_packs_epi32( g, A32 );

				r  = _mm_srai_epi32( _mm_add_epi32( z, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( z, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( z, uvB ), 10 );

				RB1 = _mm_packs_epi32( r, b );
				GA1 = _mm_packs_epi32( g, A32 );

				r  = _mm_unpacklo_epi16( RB0, RB1 );
				b  = _mm_unpackhi_epi16( RB0, RB1 );
				g  = _mm_unpacklo_epi16( GA0, GA1 );
				a  = _mm_unpackhi_epi16( GA0, GA1 );

				RB0 = _mm_unpacklo_epi16( b, r );
				RB1 = _mm_unpackhi_epi16( b, r );
				RB0 = _mm_packus_epi16( RB0, RB1 );

				GA0 = _mm_unpacklo_epi16( g, a );
				GA1 = _mm_unpackhi_epi16( g, a );
				GA0 = _mm_packus_epi16( GA0, GA1 );

				_mm_storeu_si128( ( __m128i* ) dst,  _mm_unpacklo_epi8( RB0, GA0 ) );
				dst += 16;
				_mm_storeu_si128( ( __m128i* ) dst,  _mm_unpackhi_epi8( RB0, GA0 ) );
				dst += 16;
			}
		} else {
			while( i-- ) {
				uyvy = _mm_load_si128( ( __m128i* ) src );
				src += 16;
				/* U0 Y0 V0 Z0 U1 Y1 V1 Z1 U2 Y2 V2 Z2 U3 Y3 V3 Z3 */

				uv = _mm_and_si128( mask, uyvy );
				uv = _mm_srli_si128( uv, 1 );
				uv = _mm_sub_epi16( uv, UVOFFSET ); /* U0 V0 U1 V1 ... */

				yz = _mm_andnot_si128( mask, uyvy );
				yz = _mm_sub_epi16( yz, YOFFSET );  /* Y0 Z0 Y1 Z1 ...  */

				z = _mm_madd_epi16( yz, Y2RGB );                      /* Z0 Z1 Z2 Z3 */
				y = _mm_madd_epi16( yz, _mm_srli_si128( Y2RGB, 2 ) ); /* Y0 Y1 Y2 Y3 */

				uvR = _mm_madd_epi16( uv, UV2R );
				uvG = _mm_madd_epi16( uv, UV2G );
				uvB = _mm_madd_epi16( uv, UV2B );

				r  = _mm_srai_epi32( _mm_add_epi32( y, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( y, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( y, uvB ), 10 );

				RB0 = _mm_packs_epi32( r, b );
				GA0 = _mm_packs_epi32( g, A32 );

				r  = _mm_srai_epi32( _mm_add_epi32( z, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( z, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( z, uvB ), 10 );

				RB1 = _mm_packs_epi32( r, b );
				GA1 = _mm_packs_epi32( g, A32 );

				r  = _mm_unpacklo_epi16( RB0, RB1 );
				b  = _mm_unpackhi_epi16( RB0, RB1 );
				g  = _mm_unpacklo_epi16( GA0, GA1 );
				a  = _mm_unpackhi_epi16( GA0, GA1 );

				RB0 = _mm_unpacklo_epi16( b, r );
				RB1 = _mm_unpackhi_epi16( b, r );
				RB0 = _mm_packus_epi16( RB0, RB1 );

				GA0 = _mm_unpacklo_epi16( g, a );
				GA1 = _mm_unpackhi_epi16( g, a );
				GA0 = _mm_packus_epi16( GA0, GA1 );

				_mm_stream_si128( ( __m128i* ) dst,  _mm_unpacklo_epi8( RB0, GA0 ) );
				dst += 16;
				_mm_stream_si128( ( __m128i* ) dst,  _mm_unpackhi_epi8( RB0, GA0 ) );
				dst += 16;
			}
		}

		uint32_t* dst32 = ( uint32_t* ) dst;
		uint32_t* src32 = ( uint32_t* ) src;
		i = ( n & 0x7 ) >> 1;
		while( i-- ) {
			uint32_t in, out;
			int r, g, b, y0, y1, u, v;

			in = *src32++;
			v = ( in >> 24 ) - 128;
			y1 = ( ( ( int ) ( ( in >> 16 ) & 0xff ) - 16 ) * 1192 ) >> 10;
			u = ( ( in >> 8 ) & 0xff ) - 128;
			y0 = ( ( ( int ) ( in & 0xff ) - 16 ) * 1192 ) >> 10;
			r = ((v*1634) >> 10);
			g = ((u*401 + v*832) >> 10);
			b = ((u*2066) >> 10);

			// clamp the values
			out = 0xff000000;
			out |= Math::clamp( y0 + r, 0, 255 ) << 16;
			out |= Math::clamp( y0 - g, 0, 255 ) << 8;
			out |= Math::clamp( y0 + b, 0, 255 );
			*dst32++ = out;
			out = 0xff000000;
			out |= Math::clamp( y1 + r, 0, 255 ) << 16;
			out |= Math::clamp( y1 - g, 0, 255 ) << 8;
			out |= Math::clamp( y1 + b, 0, 255 );
			*dst32++ = out;
		}
	}


	void SIMDSSE2::Conv_UYVYu8_to_RGBAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const
	{
		const __m128i Y2RGB = _mm_set_epi16( 1192, 0, 1192, 0, 1192, 0, 1192, 0 );
		const __m128i UV2R  = _mm_set_epi16( 1634, 0, 1634, 0, 1634, 0, 1634, 0 );
		const __m128i UV2G  = _mm_set_epi16( -832, -401, -832, -401, -832, -401, -832, -401 );
		const __m128i UV2B  = _mm_set_epi16( 0, 2066, 0, 2066, 0, 2066, 0, 2066 );
		const __m128i UVOFFSET = _mm_set1_epi16( 128 );
		const __m128i YOFFSET = _mm_set1_epi16( 16 );
		const __m128i A32  = _mm_set1_epi32( 0xff );
		const __m128i mask = _mm_set1_epi16( 0xff00 );

		__m128i uyvy;
		__m128i uv, yz, y, z;
		__m128i uvR, uvG, uvB;
		__m128i r, g, b, a;
		__m128i RB0, RB1, GA0, GA1;

		int i = n >> 3;
		if( ( ( size_t ) src | ( size_t ) dst ) & 0xf ) {
			while( i-- ) {
				uyvy = _mm_loadu_si128( ( __m128i* ) src );
				src += 16;
				/* U0 Y0 V0 Z0 U1 Y1 V1 Z1 U2 Y2 V2 Z2 U3 Y3 V3 Z3 */

				yz = _mm_and_si128( mask, uyvy );
				yz = _mm_srli_si128( yz, 1 );
				yz = _mm_sub_epi16( yz, YOFFSET );  /* Y0 Z0 Y1 Z1 ...  */

				uv = _mm_andnot_si128( mask, uyvy );
				uv = _mm_sub_epi16( uv, UVOFFSET ); /* U0 V0 U1 V1 ... */

				z = _mm_madd_epi16( yz, Y2RGB );                      /* Z0 Z1 Z2 Z3 */
				y = _mm_madd_epi16( yz, _mm_srli_si128( Y2RGB, 2 ) ); /* Y0 Y1 Y2 Y3 */

				uvR = _mm_madd_epi16( uv, UV2R );
				uvG = _mm_madd_epi16( uv, UV2G );
				uvB = _mm_madd_epi16( uv, UV2B );

				r  = _mm_srai_epi32( _mm_add_epi32( y, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( y, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( y, uvB ), 10 );

				RB0 = _mm_packs_epi32( r, b );
				GA0 = _mm_packs_epi32( g, A32 );

				r  = _mm_srai_epi32( _mm_add_epi32( z, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( z, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( z, uvB ), 10 );

				RB1 = _mm_packs_epi32( r, b );
				GA1 = _mm_packs_epi32( g, A32 );

				r  = _mm_unpacklo_epi16( RB0, RB1 );
				b  = _mm_unpackhi_epi16( RB0, RB1 );
				g  = _mm_unpacklo_epi16( GA0, GA1 );
				a  = _mm_unpackhi_epi16( GA0, GA1 );

				RB0 = _mm_unpacklo_epi16( r, b );
				RB1 = _mm_unpackhi_epi16( r, b );
				RB0 = _mm_packus_epi16( RB0, RB1 );

				GA0 = _mm_unpacklo_epi16( g, a );
				GA1 = _mm_unpackhi_epi16( g, a );
				GA0 = _mm_packus_epi16( GA0, GA1 );

				_mm_storeu_si128( ( __m128i* ) dst,  _mm_unpacklo_epi8( RB0, GA0 ) );
				dst += 16;
				_mm_storeu_si128( ( __m128i* ) dst,  _mm_unpackhi_epi8( RB0, GA0 ) );
				dst += 16;
			}
		} else {
			while( i-- ) {
				uyvy = _mm_load_si128( ( __m128i* ) src );
				src += 16;
				/* U0 Y0 V0 Z0 U1 Y1 V1 Z1 U2 Y2 V2 Z2 U3 Y3 V3 Z3 */

				yz = _mm_and_si128( mask, uyvy );
				yz = _mm_srli_si128( yz, 1 );
				yz = _mm_sub_epi16( yz, YOFFSET );  /* Y0 Z0 Y1 Z1 ...  */

				uv = _mm_andnot_si128( mask, uyvy );
				uv = _mm_sub_epi16( uv, UVOFFSET ); /* U0 V0 U1 V1 ... */

				z = _mm_madd_epi16( yz, Y2RGB );                      /* Z0 Z1 Z2 Z3 */
				y = _mm_madd_epi16( yz, _mm_srli_si128( Y2RGB, 2 ) ); /* Y0 Y1 Y2 Y3 */

				uvR = _mm_madd_epi16( uv, UV2R );
				uvG = _mm_madd_epi16( uv, UV2G );
				uvB = _mm_madd_epi16( uv, UV2B );

				r  = _mm_srai_epi32( _mm_add_epi32( y, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( y, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( y, uvB ), 10 );

				RB0 = _mm_packs_epi32( r, b );
				GA0 = _mm_packs_epi32( g, A32 );

				r  = _mm_srai_epi32( _mm_add_epi32( z, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( z, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( z, uvB ), 10 );

				RB1 = _mm_packs_epi32( r, b );
				GA1 = _mm_packs_epi32( g, A32 );

				r  = _mm_unpacklo_epi16( RB0, RB1 );
				b  = _mm_unpackhi_epi16( RB0, RB1 );
				g  = _mm_unpacklo_epi16( GA0, GA1 );
				a  = _mm_unpackhi_epi16( GA0, GA1 );

				RB0 = _mm_unpacklo_epi16( r, b );
				RB1 = _mm_unpackhi_epi16( r, b );
				RB0 = _mm_packus_epi16( RB0, RB1 );

				GA0 = _mm_unpacklo_epi16( g, a );
				GA1 = _mm_unpackhi_epi16( g, a );
				GA0 = _mm_packus_epi16( GA0, GA1 );

				_mm_stream_si128( ( __m128i* ) dst,  _mm_unpacklo_epi8( RB0, GA0 ) );
				dst += 16;
				_mm_stream_si128( ( __m128i* ) dst,  _mm_unpackhi_epi8( RB0, GA0 ) );
				dst += 16;
			}
		}

		uint32_t* dst32 = ( uint32_t* ) dst;
		uint32_t* src32 = ( uint32_t* ) src;
		i = ( n & 0x7 ) >> 1;
		while( i-- ) {
			uint32_t in, out;
			int r, g, b, y0, y1, u, v;

			in = *src32++;
			v = ( ( in >> 16 ) & 0xff ) - 128;
			y1 = ( ( ( int ) ( in >> 24 ) - 16 ) * 1192 ) >> 10;
			u = ( in & 0xff ) - 128;
			y0 = ( ( ( int ) ( ( in >> 8 ) & 0xff ) - 16 ) * 1192 ) >> 10;
			r = ((v*1634) >> 10);
			g = ((u*401 + v*832) >> 10);
			b = ((u*2066) >> 10);

			// clamp the values
			out = 0xff000000;
			out |= Math::clamp( y0 + r, 0, 255 );
			out |= Math::clamp( y0 - g, 0, 255 ) << 8;
			out |= Math::clamp( y0 + b, 0, 255 ) << 16;
			*dst32++ = out;
			out = 0xff000000;
			out |= Math::clamp( y1 + r, 0, 255 );
			out |= Math::clamp( y1 - g, 0, 255 ) << 8;
			out |= Math::clamp( y1 + b, 0, 255 ) << 16;
			*dst32++ = out;
		}

	}

	void SIMDSSE2::Conv_UYVYu8_to_BGRAu8( uint8_t* dst, const uint8_t* src, const size_t n ) const
	{
		const __m128i Y2RGB = _mm_set_epi16( 1192, 0, 1192, 0, 1192, 0, 1192, 0 );
		const __m128i UV2R  = _mm_set_epi16( 1634, 0, 1634, 0, 1634, 0, 1634, 0 );
		const __m128i UV2G  = _mm_set_epi16( -832, -401, -832, -401, -832, -401, -832, -401 );
		const __m128i UV2B  = _mm_set_epi16( 0, 2066, 0, 2066, 0, 2066, 0, 2066 );
		const __m128i UVOFFSET = _mm_set1_epi16( 128 );
		const __m128i YOFFSET = _mm_set1_epi16( 16 );
		const __m128i A32  = _mm_set1_epi32( 0xff );
		const __m128i mask = _mm_set1_epi16( 0xff00 );

		__m128i uyvy;
		__m128i uv, yz, y, z;
		__m128i uvR, uvG, uvB;
		__m128i r, g, b, a;
		__m128i RB0, RB1, GA0, GA1;

		int i = n >> 3;
		if( ( ( size_t ) src | ( size_t ) dst ) & 0xf ) {
			while( i-- ) {
				uyvy = _mm_loadu_si128( ( __m128i* ) src );
				src += 16;
				/* U0 Y0 V0 Z0 U1 Y1 V1 Z1 U2 Y2 V2 Z2 U3 Y3 V3 Z3 */

				yz = _mm_and_si128( mask, uyvy );
				yz = _mm_srli_si128( yz, 1 );
				yz = _mm_sub_epi16( yz, YOFFSET );  /* Y0 Z0 Y1 Z1 ...  */

				uv = _mm_andnot_si128( mask, uyvy );
				uv = _mm_sub_epi16( uv, UVOFFSET ); /* U0 V0 U1 V1 ... */

				z = _mm_madd_epi16( yz, Y2RGB );                      /* Z0 Z1 Z2 Z3 */
				y = _mm_madd_epi16( yz, _mm_srli_si128( Y2RGB, 2 ) ); /* Y0 Y1 Y2 Y3 */

				uvR = _mm_madd_epi16( uv, UV2R );
				uvG = _mm_madd_epi16( uv, UV2G );
				uvB = _mm_madd_epi16( uv, UV2B );

				r  = _mm_srai_epi32( _mm_add_epi32( y, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( y, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( y, uvB ), 10 );

				RB0 = _mm_packs_epi32( r, b );
				GA0 = _mm_packs_epi32( g, A32 );

				r  = _mm_srai_epi32( _mm_add_epi32( z, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( z, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( z, uvB ), 10 );

				RB1 = _mm_packs_epi32( r, b );
				GA1 = _mm_packs_epi32( g, A32 );

				r  = _mm_unpacklo_epi16( RB0, RB1 );
				b  = _mm_unpackhi_epi16( RB0, RB1 );
				g  = _mm_unpacklo_epi16( GA0, GA1 );
				a  = _mm_unpackhi_epi16( GA0, GA1 );

				RB0 = _mm_unpacklo_epi16( b, r );
				RB1 = _mm_unpackhi_epi16( b, r );
				RB0 = _mm_packus_epi16( RB0, RB1 );

				GA0 = _mm_unpacklo_epi16( g, a );
				GA1 = _mm_unpackhi_epi16( g, a );
				GA0 = _mm_packus_epi16( GA0, GA1 );

				_mm_storeu_si128( ( __m128i* ) dst,  _mm_unpacklo_epi8( RB0, GA0 ) );
				dst += 16;
				_mm_storeu_si128( ( __m128i* ) dst,  _mm_unpackhi_epi8( RB0, GA0 ) );
				dst += 16;
			}
		} else {
			while( i-- ) {
				uyvy = _mm_load_si128( ( __m128i* ) src );
				src += 16;
				/* U0 Y0 V0 Z0 U1 Y1 V1 Z1 U2 Y2 V2 Z2 U3 Y3 V3 Z3 */

				yz = _mm_and_si128( mask, uyvy );
				yz = _mm_srli_si128( yz, 1 );
				yz = _mm_sub_epi16( yz, YOFFSET );  /* Y0 Z0 Y1 Z1 ...  */

				uv = _mm_andnot_si128( mask, uyvy );
				uv = _mm_sub_epi16( uv, UVOFFSET ); /* U0 V0 U1 V1 ... */

				z = _mm_madd_epi16( yz, Y2RGB );                      /* Z0 Z1 Z2 Z3 */
				y = _mm_madd_epi16( yz, _mm_srli_si128( Y2RGB, 2 ) ); /* Y0 Y1 Y2 Y3 */

				uvR = _mm_madd_epi16( uv, UV2R );
				uvG = _mm_madd_epi16( uv, UV2G );
				uvB = _mm_madd_epi16( uv, UV2B );

				r  = _mm_srai_epi32( _mm_add_epi32( y, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( y, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( y, uvB ), 10 );

				RB0 = _mm_packs_epi32( r, b );
				GA0 = _mm_packs_epi32( g, A32 );

				r  = _mm_srai_epi32( _mm_add_epi32( z, uvR ), 10 );
				g  = _mm_srai_epi32( _mm_add_epi32( z, uvG ), 10 );
				b  = _mm_srai_epi32( _mm_add_epi32( z, uvB ), 10 );

				RB1 = _mm_packs_epi32( r, b );
				GA1 = _mm_packs_epi32( g, A32 );

				r  = _mm_unpacklo_epi16( RB0, RB1 );
				b  = _mm_unpackhi_epi16( RB0, RB1 );
				g  = _mm_unpacklo_epi16( GA0, GA1 );
				a  = _mm_unpackhi_epi16( GA0, GA1 );

				RB0 = _mm_unpacklo_epi16( b, r );
				RB1 = _mm_unpackhi_epi16( b, r );
				RB0 = _mm_packus_epi16( RB0, RB1 );

				GA0 = _mm_unpacklo_epi16( g, a );
				GA1 = _mm_unpackhi_epi16( g, a );
				GA0 = _mm_packus_epi16( GA0, GA1 );

				_mm_stream_si128( ( __m128i* ) dst,  _mm_unpacklo_epi8( RB0, GA0 ) );
				dst += 16;
				_mm_stream_si128( ( __m128i* ) dst,  _mm_unpackhi_epi8( RB0, GA0 ) );
				dst += 16;
			}
		}

		uint32_t* dst32 = ( uint32_t* ) dst;
		uint32_t* src32 = ( uint32_t* ) src;
		i = ( n & 0x7 ) >> 1;
		while( i-- ) {
			uint32_t in, out;
			int r, g, b, y0, y1, u, v;

			in = *src32++;
			v = ( ( in >> 16 ) & 0xff ) - 128;
			y1 = ( ( ( int ) ( in >> 24 ) - 16 ) * 1192 ) >> 10;
			u = ( in & 0xff ) - 128;
			y0 = ( ( ( int ) ( ( in >> 8 ) & 0xff ) - 16 ) * 1192 ) >> 10;
			r = ((v*1634) >> 10);
			g = ((u*401 + v*832) >> 10);
			b = ((u*2066) >> 10);

			// clamp the values
			out = 0xff000000;
			out |= Math::clamp( y0 + r, 0, 255 ) << 16;
			out |= Math::clamp( y0 - g, 0, 255 ) << 8;
			out |= Math::clamp( y0 + b, 0, 255 );
			*dst32++ = out;
			out = 0xff000000;
			out |= Math::clamp( y1 + r, 0, 255 ) << 16;
			out |= Math::clamp( y1 - g, 0, 255 ) << 8;
			out |= Math::clamp( y1 + b, 0, 255 );
			*dst32++ = out;
		}
	}

}
