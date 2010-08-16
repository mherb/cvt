#ifndef SIMD_H
#define SIMD_H
#include <stdlib.h>
#include <stdint.h>

namespace cvt {

    class SIMD
    {
	private:
	    SIMD() {};
	    SIMD( const SIMD& x ) {};

	public:
	    virtual void set_value_u8( uint8_t* dst, const size_t n, const uint8_t value ) const;
	    virtual void set_value_u16( uint16_t* dst, const size_t n, const uint16_t value ) const;
	    virtual void set_value_u32( uint32_t* dst, const size_t n, const uint32_t value ) const;
	    virtual void set_value_f( float* dst, const size_t n, const float value ) const;
	    virtual void set_value_4f( float* dst, const size_t n, const float value[ 4 ] ) const;

	    static SIMD* get();

	private:
	    static SIMD* _simd;
    };

}

#endif
