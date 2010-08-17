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
	    virtual void SetValueU8( uint8_t* dst, const size_t n, const uint8_t value ) const;
	    virtual void SetValueU16( uint16_t* dst, const size_t n, const uint16_t value ) const;
	    virtual void SetValueU32( uint32_t* dst, const size_t n, const uint32_t value ) const;
	    virtual void SetValue1F( float* dst, const size_t n, const float value ) const;
	    virtual void SetValue4f( float* dst, const size_t n, const float (&value)[ 4 ] ) const;

	    /* single precision floating point */
	    /*virtual void add_value_f( float* dst, float* src, const size_t n, float value ) const;
	    virtual void add_value4f( float* dst, float* src, const size_t n, float (&value)[ 4 ] ) const;
	    virtual void sub_valuef( float* dst, float* src, const size_t n, float value ) const;
	    virtual void sub_value4f( float* dst, float* src, const size_t n, float (&value)[ 4 ] ) const;
	    virtual void mul_value4f( float* dst, float* src, const size_t n, float (&value)[ 4 ] ) const;
	    virtual void div_valuef( float* dst, float* src, const size_t n, float value ) const;
	    virtual void div_value4f( float* dst, float* src, const size_t n, float (&value)[ 4 ] ) const;*/
	    virtual void Mul( float* dst, float* src, float value, const size_t n ) const;

	    virtual void Add( float* dst, float const* src1, float const* src2, const size_t n ) const;
	    virtual void Sub( float* dst, float const* src1, float const* src2, const size_t n ) const;
	    virtual void Mul( float* dst, float const* src1, float const* src2, const size_t n ) const;
	    virtual void Div( float* dst, float const* src1, float const* src2, const size_t n ) const;

	    virtual void MulAdd( float* dst, float const* src1, float value, const size_t n ) const;
	    virtual void MulSub( float* dst, float const* src1, float value, const size_t n ) const;
	    virtual void MulAdd( float* dst, float const* src1, float (&value)[ 4 ], const size_t n ) const;
	    virtual void MulSub( float* dst, float const* src1, float (&value)[ 4 ], const size_t n ) const;



	    virtual void conv_f_to_u8( uint8_t* dst, float* src, const size_t n );
	    virtual void conv_u8_to_f( float* dst, uint8_t* src, const size_t n );
//	    virtual void conv_srgbu8_to_f( uint8_t* dst, float* src, const size_t n );


	    static SIMD* get();

	private:
	    static SIMD* _simd;
    };

}

#endif
