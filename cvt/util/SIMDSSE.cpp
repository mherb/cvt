#include <cvt/util/SIMDSSE.h>
#include <xmmintrin.h>


namespace cvt {

/*	void SIMDSSE::Add( float* dst, float const* src1, float const* src2, const size_t n ) const
	{
		asm(
			"xorq %%rsi, %%rsi;\n"
			"movq %3, %%rdi;\n"
			"andq $(~0x0F), %%rdi;\n"
			"jz 1f;\n"
			"4:\n"
			"movaps  (%1,%%rsi,4),%%xmm0;\n"
			"movaps  (%2,%%rsi,4),%%xmm1;\n"
			"addps %%xmm1, %%xmm0;\n"
			"movntps %%xmm0,(%0,%%rsi,4);\n"
			"movaps  16(%1,%%rsi,4),%%xmm2;\n"
			"movaps  16(%2,%%rsi,4),%%xmm3;\n"
			"addps %%xmm3, %%xmm2;\n"
			"movntps %%xmm2, 16(%0,%%rsi,4);\n"
			"movaps  32(%1,%%rsi,4),%%xmm4;\n"
			"movaps  32(%2,%%rsi,4),%%xmm5;\n"
			"addps %%xmm5, %%xmm4;\n"
			"movntps %%xmm4, 32(%0,%%rsi,4);\n"
			"movaps  48(%1,%%rsi,4),%%xmm6;\n"
			"movaps  48(%2,%%rsi,4),%%xmm7;\n"
			"addps %%xmm7, %%xmm6;\n"
			"movntps %%xmm6, 48(%0,%%rsi,4);\n"
			"addq $0x10, %%rsi;\n"
			"cmpq %%rdi, %%rsi;\n"
			"jl 4b;\n"
			"andq $0xF, %3;\n"
			"leaq 1f(%%rip), %%rcx;\n"
			"movq (%%rcx,%3,8), %3;\n"
			"leaq (%3,%%rcx), %%rcx;\n"
			"jmp  *%%rcx;\n"
			".section	.rodata\n"
			".align 8\n"
			"1:\n"
			".quad	0f-1b\n"
			".quad	19f-1b\n"
			".quad	18f-1b\n"
			".quad	17f-1b\n"
			".quad	16f-1b\n"
			".quad	15f-1b\n"
			".quad	14f-1b\n"
			".quad	13f-1b\n"
			".quad	12f-1b\n"
			".quad	11f-1b\n"
			".quad	10f-1b\n"
			".quad	9f-1b\n"
			".quad	8f-1b\n"
			".quad	7f-1b\n"
			".quad	6f-1b\n"
			".quad	5f-1b\n"
			".text\n"
			".p2align 3\n"
			"5: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"6: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"7: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"8: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"9: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"10: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"11: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"12: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"13: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"14: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"15: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"16: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"17: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"18: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"19: addq $1, %%rsi;\n"
			"movss (%1,%%rsi,4), %%xmm0\n"
			"movss (%2,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%0,%%rsi,4);\n"
			"0:\n"
			:
			: "r"(dst),"r"(src1),"r"(src2),"r"(n)
			: "rcx","rsi","rdi","xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7"
		   );
	}*/


	void SIMDSSE::Add( float* dst, float const* src1, float const* src2, const size_t n ) const
	{
		size_t i = n >> 2;
        __m128 d, s1, s2;
        
        if( ( ( size_t )src1 | ( size_t )src2 | ( size_t )dst ) & 0xf ){
            while( i-- ) {
                s1 = _mm_loadu_ps( src1 );
                s2 = _mm_loadu_ps( src2 );            
                d = _mm_add_ps( s1, s2 );
                _mm_storeu_ps( dst, d );            
                
                dst += 4;
                src1 += 4;
                src2 += 4;
            }
        } else {
            while( i-- ) {
                s1 = _mm_load_ps( src1 );
                s2 = _mm_load_ps( src2 );            
                d = _mm_add_ps( s1, s2 );         
                _mm_stream_ps( dst, d );
                
                dst += 4;
                src1 += 4;
                src2 += 4;                
            }
        }
        
		i = n & 0x03;
		while( i-- )
			*dst++ = *src1++ + *src2++;
	}

	void SIMDSSE::Sub( float* dst, float const* src1, float const* src2, const size_t n ) const
	{
		size_t i = n >> 2;
        __m128 d, s1, s2;
        
        if( ( ( size_t )src1 | ( size_t )src2 | ( size_t )dst ) & 0xf ){
            while( i-- ) {
                s1 = _mm_loadu_ps( src1 );
                s2 = _mm_loadu_ps( src2 );            
                d = _mm_sub_ps( s1, s2 );
                _mm_storeu_ps( dst, d );            
                
                dst += 4;
                src1 += 4;
                src2 += 4;
            }
        } else {
            while( i-- ) {
                s1 = _mm_load_ps( src1 );
                s2 = _mm_load_ps( src2 );            
                d = _mm_sub_ps( s1, s2 );         
                _mm_stream_ps( dst, d );
                
                dst += 4;
                src1 += 4;
                src2 += 4;                
            }
        }
        
		i = n & 0x03;
		while( i-- )
			*dst++ = *src1++ - *src2++;
	}

	void SIMDSSE::Mul( float* dst, float const* src1, float const* src2, const size_t n ) const
	{
		size_t i = n >> 2;
        __m128 d, s1, s2;
        
        if( ( ( size_t )src1 | ( size_t )src2 | ( size_t )dst ) & 0xf ){
            while( i-- ) {
                s1 = _mm_loadu_ps( src1 );
                s2 = _mm_loadu_ps( src2 );            
                d = _mm_mul_ps( s1, s2 );
                _mm_storeu_ps( dst, d );            
                
                dst += 4;
                src1 += 4;
                src2 += 4;
            }
        } else {
            while( i-- ) {
                s1 = _mm_load_ps( src1 );
                s2 = _mm_load_ps( src2 );            
                d = _mm_mul_ps( s1, s2 );         
                _mm_stream_ps( dst, d );
                
                dst += 4;
                src1 += 4;
                src2 += 4;                
            }
        }
        
		i = n & 0x03;
		while( i-- )
			*dst++ = *src1++ * *src2++;
	}

	void SIMDSSE::Div( float* dst, float const* src1, float const* src2, const size_t n ) const
	{
        size_t i = n >> 2;
        __m128 d, s1, s2;
        
        if( ( ( size_t )src1 | ( size_t )src2 | ( size_t )dst ) & 0xf ){
            while( i-- ) {
                s1 = _mm_loadu_ps( src1 );
                s2 = _mm_loadu_ps( src2 );            
                d = _mm_div_ps( s1, s2 );
                _mm_storeu_ps( dst, d );            
                
                dst += 4;
                src1 += 4;
                src2 += 4;
            }
        } else {
            while( i-- ) {
                s1 = _mm_load_ps( src1 );
                s2 = _mm_load_ps( src2 );            
                d = _mm_div_ps( s1, s2 );         
                _mm_stream_ps( dst, d );
                
                dst += 4;
                src1 += 4;
                src2 += 4;                
            }
        }
        
		i = n & 0x03;
		while( i-- )
			*dst++ = *src1++ / *src2++;
	}

	void SIMDSSE::Add( float* dst, float const* src1, const float value, const size_t n ) const
	{		
        size_t i = n >> 2;
        const __m128 v = _mm_set1_ps( value );
        __m128 d, s;
        
        if( ( ( size_t )src1 | ( size_t )dst ) & 0xf ){
            while( i-- ){
                s = _mm_loadu_ps( src1 );                
                d = _mm_add_ps( s, v );                
                _mm_storeu_ps( dst, d );
                
                dst  += 4;
                src1 += 4;                
            }
        } else {
            while( i-- ){
                s = _mm_load_ps( src1 );
                d = _mm_add_ps( s, v );
                _mm_stream_ps( dst, d );
                
                dst  += 4;
                src1 += 4;  
            }
        }
        
        i = n & 0x03;
        while( i-- ){
            *dst = *src1 + value;
            dst++; src1++;
        }
	}

	void SIMDSSE::Sub( float* dst, float const* src1, const float value, const size_t n ) const
	{
        size_t i = n >> 2;
        const __m128 v = _mm_set1_ps( value );
        __m128 d, s;
        
        if( ( ( size_t )src1 | ( size_t )dst ) & 0xf ){
            while( i-- ){
                s = _mm_loadu_ps( src1 );                
                d = _mm_sub_ps( s, v );                
                _mm_storeu_ps( dst, d );
                
                dst  += 4;
                src1 += 4;                
            }
        } else {
            while( i-- ){
                s = _mm_load_ps( src1 );
                d = _mm_sub_ps( s, v );
                _mm_stream_ps( dst, d );
                
                dst  += 4;
                src1 += 4;  
            }
        }
        
        i = n & 0x03;
        while( i-- ){
            *dst = *src1 - value;
            dst++; src1++;
        }
	}

	void SIMDSSE::Mul( float* dst, float const* src1, const float value, const size_t n ) const
	{
		size_t i = n >> 2;
        const __m128 v = _mm_set1_ps( value );
        __m128 d, s;
        
        if( ( ( size_t )src1 | ( size_t )dst ) & 0xf ){
            while( i-- ){
                s = _mm_loadu_ps( src1 );                
                d = _mm_mul_ps( s, v );                
                _mm_storeu_ps( dst, d );
                
                dst  += 4;
                src1 += 4;                
            }
        } else {
            while( i-- ){
                s = _mm_load_ps( src1 );
                d = _mm_mul_ps( s, v );
                _mm_stream_ps( dst, d );
                
                dst  += 4;
                src1 += 4;  
            }
        }
        
        i = n & 0x03;
        while( i-- ){
            *dst = *src1 * value;
            dst++; src1++;
        }
	}

	void SIMDSSE::Div( float* dst, float const* src1, const float value, const size_t n ) const
	{
		size_t i = n >> 2;
        const __m128 v = _mm_set1_ps( value );
        __m128 d, s;
        
        if( ( ( size_t )src1 | ( size_t )dst ) & 0xf ){
            while( i-- ){
                s = _mm_loadu_ps( src1 );                
                d = _mm_div_ps( s, v );                
                _mm_storeu_ps( dst, d );
                
                dst  += 4;
                src1 += 4;                
            }
        } else {
            while( i-- ){
                s = _mm_load_ps( src1 );
                d = _mm_div_ps( s, v );
                _mm_stream_ps( dst, d );
                
                dst  += 4;
                src1 += 4;  
            }
        }
        
        i = n & 0x03;
        while( i-- ){
            *dst = *src1 / value;
            dst++; src1++;
        }
	}

	void SIMDSSE::MulAdd( float* dst, float const* src1, const float value, const size_t n ) const
	{ 
        if( value == 0.0f )
            return;
        
        size_t i = n >> 2;
        __m128 v = _mm_set1_ps( value );
        __m128 d, s;
        
        if( ( ( size_t )src1 | ( size_t )dst ) & 0xf ){
            while( i-- ){
                d = _mm_loadu_ps( dst );
                s = _mm_loadu_ps( src1 );
                s = _mm_mul_ps( s, v );
                d = _mm_add_ps( d, s );
                
                _mm_storeu_ps( dst, d );
                
                dst  += 4;
                src1 += 4;                
            }
        } else {
            while( i-- ){
                d = _mm_load_ps( dst );
                s = _mm_load_ps( src1 );
                s = _mm_mul_ps( s, v );
                d = _mm_add_ps( d, s );
                
                _mm_store_ps( dst, d );
                
                dst  += 4;
                src1 += 4;                
            }
        }
        
        i = n & 0x03;
        while( i-- ){
            *dst += *src1 * value;
            dst++; src1++;
        }
	}

	void SIMDSSE::MulSub( float* dst, float const* src1, const float value, const size_t n ) const
	{		
        if( value == 0.0f )
            return;
        
        size_t i = n >> 2;
        __m128 v = _mm_set1_ps( value );
        __m128 d, s;
        
        if( ( ( size_t )src1 | ( size_t )dst ) & 0xf ){
            while( i-- ){
                d = _mm_loadu_ps( dst );
                s = _mm_loadu_ps( src1 );
                s = _mm_mul_ps( s, v );
                d = _mm_sub_ps( d, s );
                
                _mm_storeu_ps( dst, d );
                
                dst  += 4;
                src1 += 4;                
            }
        } else {
            while( i-- ){
                d = _mm_load_ps( dst );
                s = _mm_load_ps( src1 );
                s = _mm_mul_ps( s, v );
                d = _mm_sub_ps( d, s );
                
                _mm_store_ps( dst, d );
                
                dst  += 4;
                src1 += 4;                
            }
        }
        
        i = n & 0x03;
        while( i-- ){
            *dst -= *src1 * value;
            dst++; src1++;
        }
	}


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
