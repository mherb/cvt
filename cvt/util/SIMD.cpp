#include <util/SIMD.h>

namespace cvt {
    SIMD* SIMD::_simd = 0;

    SIMD* SIMD::get()
    {
	if( !_simd ) {
	    _simd = new SIMD();
	}
	return _simd;
    }

    void SIMD::set_value_u8( uint8_t* dst, const size_t n, const uint8_t value ) const
    {
	size_t i = n & 0x2;
	uint32_t v = ( value << 24 ) | ( value << 16 ) | ( value << 8 ) | value;

	set_value_u32( ( uint32_t* ) dst, n >> 2, v );
	dst += n & ( ~ 0x02 );
	while( i-- )
	    *dst++ = value;
    }

    void SIMD::set_value_u16( uint16_t* dst, const size_t n, const uint16_t value ) const
    {
	uint32_t v = ( value << 16 ) | value;

	set_value_u32( ( uint32_t* ) dst, n >> 1, v );
	if( n & 1 ) {
	    dst += n & ( ~ 0x01 );
	    *dst++ = value;
	}
    }

    void SIMD::set_value_u32( uint32_t* dst, const size_t n, const uint32_t value ) const
    {
	size_t i = n;
	while( i-- )
	    *dst++ = value;
    }

    void SIMD::set_value_f( float* dst, const size_t n, const float value ) const
    {
	size_t i = n;
	while( i-- )
	    *dst++ = value;
    }

    void SIMD::set_value_4f( float* dst, const size_t n, const float value[ 4 ] ) const
    {
	size_t i = n;
	while( i-- ) {
	    *dst++ = value[ 0 ];
	    *dst++ = value[ 1 ];
	    *dst++ = value[ 2 ];
	    *dst++ = value[ 3 ];
	}
    }

}
