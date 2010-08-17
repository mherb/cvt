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

}
