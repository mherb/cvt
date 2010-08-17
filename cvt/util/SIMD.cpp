#include "util/SIMD.h"
#include "math/Math.h"

namespace cvt {
    SIMD* SIMD::_simd = 0;

    SIMD* SIMD::get()
    {
	if( !_simd ) {
	    _simd = new SIMD();
	}
	return _simd;
    }

    void SIMD::SetValueU8( uint8_t* dst, const uint8_t value, const size_t n ) const
    {
	size_t i = n & 0x2;
	uint32_t v = ( value << 24 ) | ( value << 16 ) | ( value << 8 ) | value;

	SetValueU32( ( uint32_t* ) dst, v, n >> 2 );
	dst += n & ( ~ 0x02 );
	while( i-- )
	    *dst++ = value;
    }

    void SIMD::SetValueU16( uint16_t* dst, const uint16_t value, const size_t n ) const
    {
	uint32_t v = ( value << 16 ) | value;

	SetValueU32( ( uint32_t* ) dst, v, n >> 1 );
	if( n & 1 ) {
	    dst += n & ( ~ 0x01 );
	    *dst++ = value;
	}
    }

    void SIMD::SetValueU32( uint32_t* dst, const uint32_t value, const size_t n ) const
    {
	size_t i = n;
	while( i-- )
	    *dst++ = value;
    }

    void SIMD::SetValue1f( float* dst, const float value, const size_t n ) const
    {
	size_t i = n;
	while( i-- )
	    *dst++ = value;
    }

    void SIMD::SetValue2f( float* dst, const float (&value)[ 2 ], const size_t n ) const
    {
	size_t i = n & 0x02;
	float v[ 4 ] = { value[ 0 ], value[ 1 ], value[ 0 ], value[ 1 ] };

	SetValue4f( dst, v, n >> 1 );
	while( i-- ) {
	    *dst++ = value[ 0 ];
	    *dst++ = value[ 1 ];
	}
    }

    void SIMD::SetValue4f( float* dst, const float (&value)[ 4 ], const size_t n ) const
    {
	size_t i = n;
	while( i-- ) {
	    *dst++ = value[ 0 ];
	    *dst++ = value[ 1 ];
	    *dst++ = value[ 2 ];
	    *dst++ = value[ 3 ];
	}
    }

    void SIMD::Add( float* dst, float const* src1, float const* src2, const size_t n ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = *src1++ + *src2++;
	    *dst++ = *src1++ + *src2++;
	    *dst++ = *src1++ + *src2++;
	    *dst++ = *src1++ + *src2++;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = *src1++ + *src2++;
    }

    void SIMD::Sub( float* dst, float const* src1, float const* src2, const size_t n ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = *src1++ - *src2++;
	    *dst++ = *src1++ - *src2++;
	    *dst++ = *src1++ - *src2++;
	    *dst++ = *src1++ - *src2++;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = *src1++ - *src2++;
    }

    void SIMD::Mul( float* dst, float const* src1, float const* src2, const size_t n ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = *src1++ * *src2++;
	    *dst++ = *src1++ * *src2++;
	    *dst++ = *src1++ * *src2++;
	    *dst++ = *src1++ * *src2++;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = *src1++ * *src2++;
    }

    void SIMD::Div( float* dst, float const* src1, float const* src2, const size_t n ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = *src1++ / *src2++;
	    *dst++ = *src1++ / *src2++;
	    *dst++ = *src1++ / *src2++;
	    *dst++ = *src1++ / *src2++;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = *src1++ / *src2++;
    }

    void SIMD::MulAdd( float* dst, float const* src1, float value, const size_t n ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ += *src1++ * value;
	    *dst++ += *src1++ * value;
	    *dst++ += *src1++ * value;
	    *dst++ += *src1++ * value;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ += *src1++ * value;
    }

    void SIMD::Add( float* dst, float* src, const float value, const size_t n ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = *src++ + value;
	    *dst++ = *src++ + value;
	    *dst++ = *src++ + value;
	    *dst++ = *src++ + value;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = *src++ + value;
    }

    void SIMD::Sub( float* dst, float* src, const float value, const size_t n ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = *src++ - value;
	    *dst++ = *src++ - value;
	    *dst++ = *src++ - value;
	    *dst++ = *src++ - value;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = *src++ - value;
    }

    void SIMD::Mul( float* dst, float* src, const float value, const size_t n ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = *src++ * value;
	    *dst++ = *src++ * value;
	    *dst++ = *src++ * value;
	    *dst++ = *src++ * value;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = *src++ * value;
    }

    void SIMD::Div( float* dst, float* src, const float value, const size_t n ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = *src++ / value;
	    *dst++ = *src++ / value;
	    *dst++ = *src++ / value;
	    *dst++ = *src++ / value;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = *src++ / value;
    }

    void SIMD::Add( float* dst, float const* src1, const float (&value)[ 4 ], const size_t n ) const
    {
	size_t i = n >> 2;
	size_t x = 0;
	while( i-- ) {
	    *dst++ = *src1++ + value[ 0 ];
	    *dst++ = *src1++ + value[ 1 ];
	    *dst++ = *src1++ + value[ 2 ];
	    *dst++ = *src1++ + value[ 3 ];
	}
	i = n & 0x02;
	while( i-- ) {
	    *dst++ = *src1++ + value[ x++ ];
	    x &= 0x02;
	}
    }

    void SIMD::Sub( float* dst, float const* src1, const float (&value)[ 4 ], const size_t n ) const
    {
	size_t i = n >> 2;
	size_t x = 0;
	while( i-- ) {
	    *dst++ = *src1++ - value[ 0 ];
	    *dst++ = *src1++ - value[ 1 ];
	    *dst++ = *src1++ - value[ 2 ];
	    *dst++ = *src1++ - value[ 3 ];
	}
	i = n & 0x02;
	while( i-- ) {
	    *dst++ = *src1++ - value[ x++ ];
	    x &= 0x02;
	}
    }


    void SIMD::Mul( float* dst, float const* src1, const float (&value)[ 4 ], const size_t n ) const
    {
	size_t i = n >> 2;
	size_t x = 0;
	while( i-- ) {
	    *dst++ = *src1++ * value[ 0 ];
	    *dst++ = *src1++ * value[ 1 ];
	    *dst++ = *src1++ * value[ 2 ];
	    *dst++ = *src1++ * value[ 3 ];
	}
	i = n & 0x02;
	while( i-- ) {
	    *dst++ = *src1++ * value[ x++ ];
	    x &= 0x02;
	}
    }


    void SIMD::Div( float* dst, float const* src1, const float (&value)[ 4 ], const size_t n ) const
    {
	size_t i = n >> 2;
	size_t x = 0;
	while( i-- ) {
	    *dst++ = *src1++ / value[ 0 ];
	    *dst++ = *src1++ / value[ 1 ];
	    *dst++ = *src1++ / value[ 2 ];
	    *dst++ = *src1++ / value[ 3 ];
	}
	i = n & 0x02;
	while( i-- ) {
	    *dst++ = *src1++ / value[ x++ ];
	    x &= 0x02;
	}
    }


    void SIMD::MulSub( float* dst, float const* src1, const float value, const size_t n ) const
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ -= *src1++ * value;
	    *dst++ -= *src1++ * value;
	    *dst++ -= *src1++ * value;
	    *dst++ -= *src1++ * value;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ -= *src1++ * value;
    }

    void SIMD::MulAdd( float* dst, float const* src1, const float (&value)[ 4 ], const size_t n ) const
    {
	size_t i = n >> 2;
	size_t x = 0;
	while( i-- ) {
	    *dst++ += *src1++ * value[ 0 ];
	    *dst++ += *src1++ * value[ 1 ];
	    *dst++ += *src1++ * value[ 2 ];
	    *dst++ += *src1++ * value[ 3 ];
	}
	i = n & 0x02;
	while( i-- ) {
	    *dst++ += *src1++ * value[ x++ ];
	    x &= 0x02;
	}
    }

    void SIMD::MulSub( float* dst, float const* src1, const float (&value)[ 4 ], const size_t n ) const
    {
	size_t i = n >> 2;
	size_t x = 0;
	while( i-- ) {
	    *dst++ -= *src1++ * value[ 0 ];
	    *dst++ -= *src1++ * value[ 1 ];
	    *dst++ -= *src1++ * value[ 2 ];
	    *dst++ -= *src1++ * value[ 3 ];
	}
	i = n & 0x02;
	while( i-- ) {
	    *dst++ -= *src1++ * value[ x++ ];
	    x &= 0x02;
	}
    }

    void SIMD::Conv_f_to_u8( uint8_t* dst, float* src, const size_t n )
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = ( uint8_t ) Math::clamp( *src++ * 255.0f, 0.0f, 255.0f );
	    *dst++ = ( uint8_t ) Math::clamp( *src++ * 255.0f, 0.0f, 255.0f );
	    *dst++ = ( uint8_t ) Math::clamp( *src++ * 255.0f, 0.0f, 255.0f );
	    *dst++ = ( uint8_t ) Math::clamp( *src++ * 255.0f, 0.0f, 255.0f );
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = ( uint8_t ) Math::clamp( *src++ * 255.0f, 0.0f, 255.0f );

    }

    void SIMD::Conv_u8_to_f( float* dst, uint8_t* src, const size_t n )
    {
	size_t i = n >> 2;
	while( i-- ) {
	    *dst++ = ( float ) *src++ / 255.0f;
	    *dst++ = ( float ) *src++ / 255.0f;
	    *dst++ = ( float ) *src++ / 255.0f;
	    *dst++ = ( float ) *src++ / 255.0f;
	}
	i = n & 0x02;
	while( i-- )
	    *dst++ = ( float ) *src++ / 255.0f;
    }

    void SIMD::ConvolveClampSet1f( float* dst, float const* src, const size_t width, float const* weights, const size_t wn )
    {
	float const* wp;
	float const* sp;
	float tmp;
	size_t i, k, b1, b2;

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
	i = width - wn + 1;
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

    void SIMD::ConvolveClampAdd1f( float* dst, float const* src, const size_t width, float const* weights, const size_t wn )
    {
	float const* wp;
	float const* sp;
	float tmp;
	size_t i, k, b1, b2;

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
	i = width - wn + 1;
	while( i-- ) {
	    k = wn;
	    sp = src;
	    wp = weights;
	    tmp = *sp++ * *wp++;
	    k--;
	    switch( k & 7 ) {
		case 0: while( k ) {
			    tmp += *sp++ * *wp++;
			    k--;
			    case 7:	tmp += *sp++ * *wp++;
					k--;
			    case 6:	tmp += *sp++ * *wp++;
					k--;
			    case 5:	tmp += *sp++ * *wp++;
					k--;
			    case 4:	tmp += *sp++ * *wp++;
					k--;
			    case 3:	tmp += *sp++ * *wp++;
					k--;
			    case 2:	tmp += *sp++ * *wp++;
					k--;
			    case 1:	tmp += *sp++ * *wp++;
					k--;
			}
	    }
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

    void SIMD::ConvolveClampSet2f( float* dst, float const* src, const size_t width, float const* weights, const size_t wn )
    {
	float const* wp;
	float const* sp;
	float tmp[ 2 ];
	size_t i, k, b1, b2;

	b1 = ( wn - ( 1 - ( wn & 1 ) ) ) / 2;
	b2 = ( wn + ( 1 - ( wn & 1 ) ) ) / 2;

	/* border 1 */
	i = b1;
	while( i-- ) {
	    wp = weights;
	    sp = src;
	    tmp[ 0 ] = *( sp + 0 ) * *wp;
	    tmp[ 1 ] = *( sp + 1 ) * *wp++;
	    k = i;
	    while( k-- ) {
		tmp[ 0 ] += *( sp + 0 ) * *wp;
		tmp[ 1 ] += *( sp + 1 ) * *wp++;
	    }
	    k = wn - 1 - i;
	    while( k-- ) {
		tmp[ 0 ] += *( sp + 0 ) * *wp;
		tmp[ 1 ] += *( sp + 1 ) * *wp++;
		sp += 2;
	    }
	    *dst++ = tmp[ 0 ];
	    *dst++ = tmp[ 1 ];
	    *dst++ = tmp[ 2 ];
	    *dst++ = tmp[ 3 ];
	}


	/* center */
	i = width - wn + 1;
	while( i-- ) {
	    k = wn;
	    sp = src;
	    wp = weights;
	    tmp[ 0 ] = *( sp + 0 ) * *wp;
	    tmp[ 1 ] = *( sp + 1 ) * *wp++;
	    sp += 2;
	    k--;
	    while( k-- ) {
		tmp[ 0 ] += *( sp + 0 ) * *wp;
		tmp[ 1 ] += *( sp + 1 ) * *wp++;
		sp += 2;
	    }
	    *dst++ = tmp[ 0 ];
	    *dst++ = tmp[ 1 ];
	    src += 2;
	}

	/* border 2 */
	i = b2;
	while( i-- ) {
	    wp = weights;
	    sp = src;
	    tmp[ 0 ] = *( sp + 0 ) * *wp;
	    tmp[ 1 ] = *( sp + 1 ) * *wp++;
	    k = b1 + i;
	    while( k-- ) {
		sp += 2;
		tmp[ 0 ] += *( sp + 0 ) * *wp;
		tmp[ 1 ] += *( sp + 1 ) * *wp++;
	    }
	    k = b2 - i;
	    while( k-- ) {
		tmp[ 0 ] += *( sp + 0 ) * *wp;
		tmp[ 1 ] += *( sp + 1 ) * *wp++;
	    }
	    *dst++ = tmp[ 0 ];
	    *dst++ = tmp[ 1 ];
	    src += 2;
	}
    }

    void SIMD::ConvolveClampAdd2f( float* dst, float const* src, const size_t width, float const* weights, const size_t wn )
    {
	float const* wp;
	float const* sp;
	float tmp[ 2 ];
	size_t i, k, b1, b2;

	b1 = ( wn - ( 1 - ( wn & 1 ) ) ) / 2;
	b2 = ( wn + ( 1 - ( wn & 1 ) ) ) / 2;

	/* border 1 */
	i = b1;
	while( i-- ) {
	    wp = weights;
	    sp = src;
	    tmp[ 0 ] = *( sp + 0 ) * *wp;
	    tmp[ 1 ] = *( sp + 1 ) * *wp++;
	    k = i;
	    while( k-- ) {
		tmp[ 0 ] += *( sp + 0 ) * *wp;
		tmp[ 1 ] += *( sp + 1 ) * *wp++;
	    }
	    k = wn - 1 - i;
	    while( k-- ) {
		tmp[ 0 ] += *( sp + 0 ) * *wp;
		tmp[ 1 ] += *( sp + 1 ) * *wp++;
		sp += 2;
	    }
	    *dst++ += tmp[ 0 ];
	    *dst++ += tmp[ 1 ];
	}


	/* center */
	i = width - wn + 1;
	while( i-- ) {
	    float w;
	    k = wn;
	    sp = src;
	    wp = weights;
	    w = *wp++;
	    tmp[ 0 ] = *( sp + 0 ) * w;
	    tmp[ 1 ] = *( sp + 1 ) * w;
	    sp += 2;
	    k--;
	    switch( k & 7 ) {
		case 0: while( k ) {
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    sp += 2;
			    k--;
			    case 7:
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    sp += 2;
			    k--;
			    case 6:
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    sp += 2;
			    k--;
			    case 5:
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    sp += 2;
			    k--;
			    case 4:
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    sp += 2;
			    k--;
			    case 3:
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    sp += 2;
			    k--;
			    case 2:
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    sp += 2;
			    k--;
			    case 1:
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    sp += 2;
			    k--;
			}
	    }
	    *dst++ += tmp[ 0 ];
	    *dst++ += tmp[ 1 ];
	    src += 2;
	}

	/* border 2 */
	i = b2;
	while( i-- ) {
	    wp = weights;
	    sp = src;
	    tmp[ 0 ] = *( sp + 0 ) * *wp;
	    tmp[ 1 ] = *( sp + 1 ) * *wp++;
	    k = b1 + i;
	    while( k-- ) {
		sp += 2;
		tmp[ 0 ] += *( sp + 0 ) * *wp;
		tmp[ 1 ] += *( sp + 1 ) * *wp++;
	    }
	    k = b2 - i;
	    while( k-- ) {
		tmp[ 0 ] += *( sp + 0 ) * *wp;
		tmp[ 1 ] += *( sp + 1 ) * *wp++;
	    }
	    *dst++ += tmp[ 0 ];
	    *dst++ += tmp[ 1 ];
	    src += 2;
	}
    }

    void SIMD::ConvolveClampSet4f( float* dst, float const* src, const size_t width, float const* weights, const size_t wn )
    {
	float const* wp;
	float const* sp;
	float tmp[ 4 ];
	size_t i, k, b1, b2;

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
	i = width - wn + 1;
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

    void SIMD::ConvolveClampAdd4f( float* dst, float const* src, const size_t width, float const* weights, const size_t wn )
    {
	float const* wp;
	float const* sp;
	float tmp[ 4 ];
	size_t i, k, b1, b2;

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
	i = width - wn + 1;
	while( i-- ) {
	    float w;
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
	    switch( k & 7 ) {
		case 0: while( k ) {
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    tmp[ 2 ] += *( sp + 2 ) * w;
			    tmp[ 3 ] += *( sp + 3 ) * w;
			    sp += 4;
			    k--;
			    case 7:
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    tmp[ 2 ] += *( sp + 2 ) * w;
			    tmp[ 3 ] += *( sp + 3 ) * w;
			    sp += 4;
			    k--;
			    case 6:
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    tmp[ 2 ] += *( sp + 2 ) * w;
			    tmp[ 3 ] += *( sp + 3 ) * w;
			    sp += 4;
			    k--;
			    case 5:
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    tmp[ 2 ] += *( sp + 2 ) * w;
			    tmp[ 3 ] += *( sp + 3 ) * w;
			    sp += 4;
			    k--;
			    case 4:
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    tmp[ 2 ] += *( sp + 2 ) * w;
			    tmp[ 3 ] += *( sp + 3 ) * w;
			    sp += 4;
			    k--;
			    case 3:
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    tmp[ 2 ] += *( sp + 2 ) * w;
			    tmp[ 3 ] += *( sp + 3 ) * w;
			    sp += 4;
			    k--;
			    case 2:
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    tmp[ 2 ] += *( sp + 2 ) * w;
			    tmp[ 3 ] += *( sp + 3 ) * w;
			    sp += 4;
			    k--;
			    case 1:
			    w = *wp++;
			    tmp[ 0 ] += *( sp + 0 ) * w;
			    tmp[ 1 ] += *( sp + 1 ) * w;
			    tmp[ 2 ] += *( sp + 2 ) * w;
			    tmp[ 3 ] += *( sp + 3 ) * w;
			    sp += 4;
			    k--;
			}
	    }
	    /*	while( k-- ) {
		tmp[ 0 ] += *( sp + 0 ) * *wp;
		tmp[ 1 ] += *( sp + 1 ) * *wp;
		tmp[ 2 ] += *( sp + 2 ) * *wp;
		tmp[ 3 ] += *( sp + 3 ) * *wp++;
		sp += 4;
		}*/
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

}
