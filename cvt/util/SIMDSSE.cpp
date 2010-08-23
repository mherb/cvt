#include "util/SIMDSSE.h"
#include <xmmintrin.h>


namespace cvt {

	void SIMDSSE::Add( float* dst, float const* src1, float const* src2, const size_t n ) const
	{
		asm(
			"xorq %%rsi, %%rsi;\n"
			"movq %%rdx, %%rdi;\n"
			"andq $0xFFFFFFFFFFFFFFF0, %%rdi;\n"
			"jz loop1;\n"
			"loop4:\n"
			"PREFETCHNTA 32(%%rbx,%%rsi,4);\n"
			"movaps  (%%rbx,%%rsi,4),%%xmm0;\n"
			"movaps  (%%rcx,%%rsi,4),%%xmm1;\n"
			"addps %%xmm1, %%xmm0;\n"
			"movaps %%xmm0,(%%rax,%%rsi,4);\n"
			"movaps  16(%%rbx,%%rsi,4),%%xmm2;\n"
			"movaps  16(%%rcx,%%rsi,4),%%xmm3;\n"
			"addps %%xmm3, %%xmm2;\n"
			"PREFETCHNTA 64(%%rcx,%%rsi,4);\n"
			"movaps %%xmm2, 16(%%rax,%%rsi,4);\n"
			"movaps  32(%%rbx,%%rsi,4),%%xmm4;\n"
			"movaps  32(%%rcx,%%rsi,4),%%xmm5;\n"
			"addps %%xmm5, %%xmm4;\n"
			"movaps %%xmm4, 32(%%rax,%%rsi,4);\n"
			"movaps  48(%%rbx,%%rsi,4),%%xmm6;\n"
			"movaps  48(%%rcx,%%rsi,4),%%xmm7;\n"
			"addps %%xmm7, %%xmm6;\n"
			"movaps %%xmm6, 48(%%rax,%%rsi,4);\n"
			"addq $0x10, %%rsi;\n"
			"cmpq %%rdi, %%rsi;\n"
			"jl loop4;\n"
			"testq $0xF, %%rdx;\n"
			"jz end;\n"
			"\n"
			"loop1:\n"
			"movss (%%rbx,%%rsi,4), %%xmm0\n"
			"movss (%%rcx,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%%rax,%%rsi,4);\n"
			"addq $1, %%rsi;\n"
			"cmpq %%rdx, %%rsi;\n"
			"jl loop1;\n"
			"end:\n"
			:
			: "a"(dst),"b"(src1),"c"(src2),"d"(n)
			: "rsi","rdi","xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7"
		   );
	}

	void SIMDSSE::Sub( float* dst, float const* src1, float const* src2, const size_t n ) const
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

	void SIMDSSE::Mul( float* dst, float const* src1, float const* src2, const size_t n ) const
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

	void SIMDSSE::Div( float* dst, float const* src1, float const* src2, const size_t n ) const
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

}
