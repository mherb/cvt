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
	    /* set to constant */
	    virtual void SetValueU8( uint8_t* dst, const uint8_t value, const size_t n ) const;
	    virtual void SetValueU16( uint16_t* dst, const uint16_t value, const size_t n ) const;
	    virtual void SetValueU32( uint32_t* dst, const uint32_t value, const size_t n ) const;
	    virtual void SetValue1f( float* dst, const float value, const size_t n ) const;
	    virtual void SetValue2f( float* dst, const float (&value)[ 2 ], const size_t n ) const;
	    virtual void SetValue4f( float* dst, const float (&value)[ 4 ], const size_t n ) const;

	    /* memory block and constants */
	    virtual void Add( float* dst, float* src, const float value, const size_t n ) const;
	    virtual void Sub( float* dst, float* src, const float value, const size_t n ) const;
	    virtual void Mul( float* dst, float* src, const float value, const size_t n ) const;
	    virtual void Div( float* dst, float* src, const float value, const size_t n ) const;

	    virtual void Add( float* dst, const float* src, const float (&value)[ 4 ], const size_t n ) const;
	    virtual void Sub( float* dst, const float* src, const float (&value)[ 4 ], const size_t n ) const;
	    virtual void Mul( float* dst, const float* src, const float (&value)[ 4 ], const size_t n ) const;
	    virtual void Div( float* dst, const float* src, const float (&value)[ 4 ], const size_t n ) const;

	    virtual void MulAdd( float* dst, float const* src1, const float value, const size_t n ) const;
	    virtual void MulSub( float* dst, float const* src1, const float value, const size_t n ) const;

	    virtual void MulAdd( float* dst, float const* src1, const float (&value)[ 4 ], const size_t n ) const;
	    virtual void MulSub( float* dst, float const* src1, const float (&value)[ 4 ], const size_t n ) const;

	    /* memory blocks */
	    virtual void Add( float* dst, float const* src1, float const* src2, const size_t n ) const;
	    virtual void Sub( float* dst, float const* src1, float const* src2, const size_t n ) const;
	    virtual void Mul( float* dst, float const* src1, float const* src2, const size_t n ) const;
	    virtual void Div( float* dst, float const* src1, float const* src2, const size_t n ) const;


	    void ConvolveClampSet1f( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn );
	    void ConvolveClampAdd1f( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn );
	    void ConvolveClampSet4f( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn );
	    void ConvolveClampAdd4f( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn );



	    virtual void Conv_f_to_u8( uint8_t* dst, float* src, const size_t n );
	    virtual void Conv_u8_to_f( float* dst, uint8_t* src, const size_t n );
//	    virtual void conv_srgbu8_to_f( uint8_t* dst, float* src, const size_t n );


	    static SIMD* get();

	private:
	    static SIMD* _simd;
    };

}

#endif
