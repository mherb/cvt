#include "util/SIMDSSE.h"
#include <xmmintrin.h>


namespace cvt {

	void SIMDSSE::Add( float* dst, float const* src1, float const* src2, const size_t n ) const
	{
		asm(
			"xorq %%rsi, %%rsi;\n"
			"movq %%rdx, %%rdi;\n"
			"andq $0xFFFFFFFFFFFFFFF0, %%rdi;\n"
			"jz 1f;\n"
			"4:\n"
			"movaps  (%%rbx,%%rsi,4),%%xmm0;\n"
			"movaps  (%%rcx,%%rsi,4),%%xmm1;\n"
			"addps %%xmm1, %%xmm0;\n"
			"movntps %%xmm0,(%%rax,%%rsi,4);\n"
			"movaps  16(%%rbx,%%rsi,4),%%xmm2;\n"
			"movaps  16(%%rcx,%%rsi,4),%%xmm3;\n"
			"addps %%xmm3, %%xmm2;\n"
			"movntps %%xmm2, 16(%%rax,%%rsi,4);\n"
			"movaps  32(%%rbx,%%rsi,4),%%xmm4;\n"
			"movaps  32(%%rcx,%%rsi,4),%%xmm5;\n"
			"addps %%xmm5, %%xmm4;\n"
			"movntps %%xmm4, 32(%%rax,%%rsi,4);\n"
			"movaps  48(%%rbx,%%rsi,4),%%xmm6;\n"
			"movaps  48(%%rcx,%%rsi,4),%%xmm7;\n"
			"addps %%xmm7, %%xmm6;\n"
			"movntps %%xmm6, 48(%%rax,%%rsi,4);\n"
			"addq $0x10, %%rsi;\n"
			"cmpq %%rdi, %%rsi;\n"
			"jl 4b;\n"
			"testq $0xF, %%rdx;\n"
			"jz 0f;\n"
			"\n"
			"1:\n"
			"movss (%%rbx,%%rsi,4), %%xmm0\n"
			"movss (%%rcx,%%rsi,4), %%xmm1\n"
			"addps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%%rax,%%rsi,4);\n"
			"addq $1, %%rsi;\n"
			"cmpq %%rdx, %%rsi;\n"
			"jl 1b;\n"
			"0:\n"
			:
			: "a"(dst),"b"(src1),"c"(src2),"d"(n)
			: "rsi","rdi","xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7"
		   );
	}

	void SIMDSSE::Sub( float* dst, float const* src1, float const* src2, const size_t n ) const
	{
		asm(
			"xorq %%rsi, %%rsi;\n"
			"movq %%rdx, %%rdi;\n"
			"andq $0xFFFFFFFFFFFFFFF0, %%rdi;\n"
			"jz 1f;\n"
			"4:\n"
			"movaps  (%%rbx,%%rsi,4),%%xmm0;\n"
			"movaps  (%%rcx,%%rsi,4),%%xmm1;\n"
			"subps %%xmm1, %%xmm0;\n"
			"movntps %%xmm0,(%%rax,%%rsi,4);\n"
			"movaps  16(%%rbx,%%rsi,4),%%xmm2;\n"
			"movaps  16(%%rcx,%%rsi,4),%%xmm3;\n"
			"subps %%xmm3, %%xmm2;\n"
			"movntps %%xmm2, 16(%%rax,%%rsi,4);\n"
			"movaps  32(%%rbx,%%rsi,4),%%xmm4;\n"
			"movaps  32(%%rcx,%%rsi,4),%%xmm5;\n"
			"subps %%xmm5, %%xmm4;\n"
			"movntps %%xmm4, 32(%%rax,%%rsi,4);\n"
			"movaps  48(%%rbx,%%rsi,4),%%xmm6;\n"
			"movaps  48(%%rcx,%%rsi,4),%%xmm7;\n"
			"subps %%xmm7, %%xmm6;\n"
			"movntps %%xmm6, 48(%%rax,%%rsi,4);\n"
			"addq $0x10, %%rsi;\n"
			"cmpq %%rdi, %%rsi;\n"
			"jl 4b;\n"
			"testq $0xF, %%rdx;\n"
			"jz 0f;\n"
			"\n"
			"1:\n"
			"movss (%%rbx,%%rsi,4), %%xmm0\n"
			"movss (%%rcx,%%rsi,4), %%xmm1\n"
			"subps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%%rax,%%rsi,4);\n"
			"addq $1, %%rsi;\n"
			"cmpq %%rdx, %%rsi;\n"
			"jl 1b;\n"
			"0:\n"
			:
			: "a"(dst),"b"(src1),"c"(src2),"d"(n)
			: "rsi","rdi","xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7"
		   );
	}

	void SIMDSSE::Mul( float* dst, float const* src1, float const* src2, const size_t n ) const
	{
		asm(
			"xorq %%rsi, %%rsi;\n"
			"movq %%rdx, %%rdi;\n"
			"andq $0xFFFFFFFFFFFFFFF0, %%rdi;\n"
			"jz 1f;\n"
			"4:\n"
			"movaps  (%%rbx,%%rsi,4),%%xmm0;\n"
			"movaps  (%%rcx,%%rsi,4),%%xmm1;\n"
			"mulps %%xmm1, %%xmm0;\n"
			"movntps %%xmm0,(%%rax,%%rsi,4);\n"
			"movaps  16(%%rbx,%%rsi,4),%%xmm2;\n"
			"movaps  16(%%rcx,%%rsi,4),%%xmm3;\n"
			"mulps %%xmm3, %%xmm2;\n"
			"movntps %%xmm2, 16(%%rax,%%rsi,4);\n"
			"movaps  32(%%rbx,%%rsi,4),%%xmm4;\n"
			"movaps  32(%%rcx,%%rsi,4),%%xmm5;\n"
			"mulps %%xmm5, %%xmm4;\n"
			"movntps %%xmm4, 32(%%rax,%%rsi,4);\n"
			"movaps  48(%%rbx,%%rsi,4),%%xmm6;\n"
			"movaps  48(%%rcx,%%rsi,4),%%xmm7;\n"
			"mulps %%xmm7, %%xmm6;\n"
			"movntps %%xmm6, 48(%%rax,%%rsi,4);\n"
			"addq $0x10, %%rsi;\n"
			"cmpq %%rdi, %%rsi;\n"
			"jl 4b;\n"
			"testq $0xF, %%rdx;\n"
			"jz 0f;\n"
			"\n"
			"1:\n"
			"movss (%%rbx,%%rsi,4), %%xmm0\n"
			"movss (%%rcx,%%rsi,4), %%xmm1\n"
			"mulps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%%rax,%%rsi,4);\n"
			"addq $1, %%rsi;\n"
			"cmpq %%rdx, %%rsi;\n"
			"jl 1b;\n"
			"0:\n"
			:
			: "a"(dst),"b"(src1),"c"(src2),"d"(n)
			: "rsi","rdi","xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7"
		   );
	}

	void SIMDSSE::Div( float* dst, float const* src1, float const* src2, const size_t n ) const
	{
		asm(
			"xorq %%rsi, %%rsi;\n"
			"movq %%rdx, %%rdi;\n"
			"andq $0xFFFFFFFFFFFFFFF0, %%rdi;\n"
			"jz 1f;\n"
			"4:\n"
			"movaps  (%%rbx,%%rsi,4),%%xmm0;\n"
			"movaps  (%%rcx,%%rsi,4),%%xmm1;\n"
			"divps %%xmm1, %%xmm0;\n"
			"movntps %%xmm0,(%%rax,%%rsi,4);\n"
			"movaps  16(%%rbx,%%rsi,4),%%xmm2;\n"
			"movaps  16(%%rcx,%%rsi,4),%%xmm3;\n"
			"divps %%xmm3, %%xmm2;\n"
			"movntps %%xmm2, 16(%%rax,%%rsi,4);\n"
			"movaps  32(%%rbx,%%rsi,4),%%xmm4;\n"
			"movaps  32(%%rcx,%%rsi,4),%%xmm5;\n"
			"divps %%xmm5, %%xmm4;\n"
			"movntps %%xmm4, 32(%%rax,%%rsi,4);\n"
			"movaps  48(%%rbx,%%rsi,4),%%xmm6;\n"
			"movaps  48(%%rcx,%%rsi,4),%%xmm7;\n"
			"divps %%xmm7, %%xmm6;\n"
			"movntps %%xmm6, 48(%%rax,%%rsi,4);\n"
			"addq $0x10, %%rsi;\n"
			"cmpq %%rdi, %%rsi;\n"
			"jl 4b;\n"
			"testq $0xF, %%rdx;\n"
			"jz 0f;\n"
			"\n"
			"1:\n"
			"movss (%%rbx,%%rsi,4), %%xmm0\n"
			"movss (%%rcx,%%rsi,4), %%xmm1\n"
			"divps %%xmm1, %%xmm0;\n"
			"movss %%xmm0,(%%rax,%%rsi,4);\n"
			"addq $1, %%rsi;\n"
			"cmpq %%rdx, %%rsi;\n"
			"jl 1b;\n"
			"0:\n"
			:
			: "a"(dst),"b"(src1),"c"(src2),"d"(n)
			: "rsi","rdi","xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7"
		   );
	}

}
