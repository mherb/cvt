#include "util/SIMDSSE.h"


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

#if defined(__x86_64__)
#define OPFAA(d,s1,s2,n,aluop) asm(					\
			"xorq %%rsi, %%rsi;\n"					\
			"movq %3, %%rdi;\n"						\
			"andq $(~0x0f), %%rdi;\n"				\
			"jz 5f;\n"								\
			"4:\n"									\
			"movaps  (%1,%%rsi,4),%%xmm0;\n"		\
			"movaps  (%2,%%rsi,4),%%xmm1;\n"		\
			#aluop " %%xmm1, %%xmm0;\n"				\
			"movntps %%xmm0,(%0,%%rsi,4);\n"		\
			"movaps  16(%1,%%rsi,4),%%xmm2;\n"		\
			"movaps  16(%2,%%rsi,4),%%xmm3;\n"		\
			#aluop " %%xmm3, %%xmm2;\n"				\
			"movntps %%xmm2, 16(%0,%%rsi,4);\n"		\
			"movaps  32(%1,%%rsi,4),%%xmm4;\n"		\
			"movaps  32(%2,%%rsi,4),%%xmm5;\n"		\
			#aluop " %%xmm5, %%xmm4;\n"             \
			"movntps %%xmm4, 32(%0,%%rsi,4);\n"		\
			"movaps  48(%1,%%rsi,4),%%xmm6;\n"		\
			"movaps  48(%2,%%rsi,4),%%xmm7;\n"		\
			#aluop " %%xmm7, %%xmm6;\n"             \
			"movntps %%xmm6, 48(%0,%%rsi,4);\n"		\
			"addq $0x10, %%rsi;\n"                  \
			"cmpq %%rdi, %%rsi;\n"                  \
			"jl 4b;\n"                              \
			"5:\n"									\
			"testq $0xf, %3;\n"						\
			"jz 0f;\n"                              \
			"\n"                                    \
			"1:\n"                                  \
			"movss (%1,%%rsi,4), %%xmm0\n"			\
			"movss (%2,%%rsi,4), %%xmm1\n"			\
			#aluop " %%xmm1, %%xmm0;\n"				\
			"movss %%xmm0,(%0,%%rsi,4);\n"			\
			"addq $1, %%rsi;\n"                     \
			"cmpq %3, %%rsi;\n"						\
			"jl 1b;\n"                              \
			"0:\n"                                  \
			:                                       \
			: "r"(d),"r"(s1),"r"(s2),"r"(n)   \
			: "rsi","rdi","xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7" \
		   );


#define OPFAC1(d,s1,v,n,aluop) asm(					\
			"xorq %%rsi, %%rsi;\n"					\
			"movss %2,%%xmm0;\n"					\
			"shufps $0, %%xmm0, %%xmm0;\n"			\
			"movq %3, %%rdi;\n"						\
			"andq $(~0x0f), %%rdi;\n"				\
			"jz 5f;\n"								\
			"4:\n"									\
			"movaps  (%1,%%rsi,4),%%xmm1;\n"		\
			#aluop " %%xmm0, %%xmm1;\n"				\
			"movntps %%xmm1,(%0,%%rsi,4);\n"		\
			"movaps  16(%1,%%rsi,4),%%xmm2;\n"		\
			#aluop " %%xmm0, %%xmm2;\n"				\
			"movntps %%xmm2, 16(%0,%%rsi,4);\n"		\
			"movaps  32(%1,%%rsi,4),%%xmm3;\n"		\
			#aluop " %%xmm0, %%xmm3;\n"             \
			"movntps %%xmm3, 32(%0,%%rsi,4);\n"		\
			"movaps  48(%1,%%rsi,4),%%xmm4;\n"		\
			#aluop " %%xmm0, %%xmm4;\n"             \
			"movntps %%xmm4, 48(%0,%%rsi,4);\n"		\
			"addq $0x10, %%rsi;\n"                  \
			"cmpq %%rdi, %%rsi;\n"                  \
			"jl 4b;\n"                              \
			"5:\n"									\
			"testq $0xf, %3;\n"						\
			"jz 0f;\n"                              \
			"\n"                                    \
			"1:\n"                                  \
			"movss (%1,%%rsi,4), %%xmm1\n"			\
			#aluop " %%xmm0, %%xmm1;\n"				\
			"movss %%xmm1,(%0,%%rsi,4);\n"			\
			"addq $1, %%rsi;\n"                     \
			"cmpq %3, %%rsi;\n"						\
			"jl 1b;\n"                              \
			"0:\n"                                  \
			:                                       \
			: "r"(d),"r"(s1),"m"(v),"r"(n)			\
			: "rsi","rdi","xmm0","xmm1","xmm2","xmm3","xmm4" \
		   );

#define OPF2AC1(d,s1,v,n,aluop,aluop2) asm(			\
			"xorq %%rsi, %%rsi;\n"					\
			"movss %2,%%xmm0;\n"					\
			"shufps $0, %%xmm0, %%xmm0;\n"			\
			"movq %3, %%rdi;\n"						\
			"andq $(~0x0f), %%rdi;\n"				\
			"jz 5f;\n"								\
			"4:\n"									\
			"movaps  (%0,%%rsi,4),%%xmm1;\n"		\
			"movaps  (%1,%%rsi,4),%%xmm2;\n"		\
			#aluop " %%xmm0, %%xmm2;\n"				\
			#aluop2 " %%xmm2, %%xmm1;\n"			\
			"movntps %%xmm1,(%0,%%rsi,4);\n"		\
			"movaps  16(%0,%%rsi,4),%%xmm3;\n"		\
			"movaps  16(%1,%%rsi,4),%%xmm4;\n"		\
			#aluop " %%xmm0, %%xmm4;\n"				\
			#aluop2 " %%xmm4, %%xmm3;\n"			\
			"movntps %%xmm3, 16(%0,%%rsi,4);\n"		\
			"movaps  32(%0,%%rsi,4),%%xmm5;\n"		\
			"movaps  32(%1,%%rsi,4),%%xmm6;\n"		\
			#aluop " %%xmm0, %%xmm6;\n"				\
			#aluop2 " %%xmm6, %%xmm5;\n"            \
			"movntps %%xmm5, 32(%0,%%rsi,4);\n"		\
			"movaps  48(%0,%%rsi,4),%%xmm7;\n"		\
			"movaps  48(%1,%%rsi,4),%%xmm1;\n"		\
			#aluop " %%xmm0, %%xmm1;\n"				\
			#aluop2 " %%xmm1, %%xmm7;\n"            \
			"movntps %%xmm7, 48(%0,%%rsi,4);\n"		\
			"addq $0x10, %%rsi;\n"                  \
			"cmpq %%rdi, %%rsi;\n"                  \
			"jl 4b;\n"                              \
			"5:\n"									\
			"testq $0xf, %3;\n"						\
			"jz 0f;\n"                              \
			"\n"                                    \
			"1:\n"                                  \
			"movss (%0,%%rsi,4), %%xmm1\n"			\
			"movss (%1,%%rsi,4), %%xmm2\n"			\
			#aluop " %%xmm0, %%xmm2;\n"				\
			#aluop2 " %%xmm2, %%xmm1;\n"			\
			"movss %%xmm1,(%0,%%rsi,4);\n"			\
			"addq $1, %%rsi;\n"                     \
			"cmpq %3, %%rsi;\n"						\
			"jl 1b;\n"                              \
			"0:\n"                                  \
			:                                       \
			: "r"(d),"r"(s1),"m"(v),"r"(n)   \
			: "rsi","rdi","xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7" \
		   );

#else
#define OPFAA(d,s1,s2,n,aluop) asm(					\
			"xorq %%esi, %%esi;\n"					\
			"movq %3, %%edi;\n"						\
			"andq $(~0x0f), %%edi;\n"				\
			"jz 5f;\n"								\
			"4:\n"									\
			"movaps  (%1,%%esi,4),%%xmm0;\n"		\
			"movaps  (%2,%%esi,4),%%xmm1;\n"		\
			#aluop " %%xmm1, %%xmm0;\n"				\
			"movntps %%xmm0,(%0,%%esi,4);\n"		\
			"movaps  16(%1,%%esi,4),%%xmm2;\n"		\
			"movaps  16(%2,%%esi,4),%%xmm3;\n"		\
			#aluop " %%xmm3, %%xmm2;\n"				\
			"movntps %%xmm2, 16(%0,%%esi,4);\n"		\
			"movaps  32(%1,%%esi,4),%%xmm4;\n"		\
			"movaps  32(%2,%%esi,4),%%xmm5;\n"		\
			#aluop " %%xmm5, %%xmm4;\n"             \
			"movntps %%xmm4, 32(%0,%%esi,4);\n"		\
			"movaps  48(%1,%%esi,4),%%xmm6;\n"		\
			"movaps  48(%2,%%esi,4),%%xmm7;\n"		\
			#aluop " %%xmm7, %%xmm6;\n"             \
			"movntps %%xmm6, 48(%0,%%esi,4);\n"		\
			"addq $0x10, %%esi;\n"                  \
			"cmpq %%edi, %%esi;\n"                  \
			"jl 4b;\n"                              \
			"5:\n"									\
			"testq $0xf, %3;\n"						\
			"jz 0f;\n"                              \
			"\n"                                    \
			"1:\n"                                  \
			"movss (%1,%%esi,4), %%xmm0\n"			\
			"movss (%2,%%esi,4), %%xmm1\n"			\
			#aluop " %%xmm1, %%xmm0;\n"				\
			"movss %%xmm0,(%0,%%esi,4);\n"			\
			"addq $1, %%esi;\n"                     \
			"cmpq %3, %%esi;\n"						\
			"jl 1b;\n"                              \
			"0:\n"                                  \
			:                                       \
			: "r"(d),"r"(s1),"r"(s2),"r"(n)   \
			: "rsi","edi","xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7" \
		   );


#define OPFAC1(d,s1,v,n,aluop) asm(					\
			"xorq %%esi, %%esi;\n"					\
			"movss %2,%%xmm0;\n"					\
			"shufps $0, %%xmm0, %%xmm0;\n"			\
			"movq %3, %%edi;\n"						\
			"andq $(~0x0f), %%edi;\n"				\
			"jz 5f;\n"								\
			"4:\n"									\
			"movaps  (%1,%%esi,4),%%xmm1;\n"		\
			#aluop " %%xmm0, %%xmm1;\n"				\
			"movntps %%xmm1,(%0,%%esi,4);\n"		\
			"movaps  16(%1,%%esi,4),%%xmm2;\n"		\
			#aluop " %%xmm0, %%xmm2;\n"				\
			"movntps %%xmm2, 16(%0,%%esi,4);\n"		\
			"movaps  32(%1,%%esi,4),%%xmm3;\n"		\
			#aluop " %%xmm0, %%xmm3;\n"             \
			"movntps %%xmm3, 32(%0,%%esi,4);\n"		\
			"movaps  48(%1,%%esi,4),%%xmm4;\n"		\
			#aluop " %%xmm0, %%xmm4;\n"             \
			"movntps %%xmm4, 48(%0,%%esi,4);\n"		\
			"addq $0x10, %%esi;\n"                  \
			"cmpq %%edi, %%esi;\n"                  \
			"jl 4b;\n"                              \
			"5:\n"									\
			"testq $0xf, %3;\n"						\
			"jz 0f;\n"                              \
			"\n"                                    \
			"1:\n"                                  \
			"movss (%1,%%esi,4), %%xmm1\n"			\
			#aluop " %%xmm0, %%xmm1;\n"				\
			"movss %%xmm1,(%0,%%esi,4);\n"			\
			"addq $1, %%esi;\n"                     \
			"cmpq %3, %%esi;\n"						\
			"jl 1b;\n"                              \
			"0:\n"                                  \
			:                                       \
			: "r"(d),"r"(s1),"m"(v),"r"(n)			\
			: "esi","edi","xmm0","xmm1","xmm2","xmm3","xmm4" \
		   );
#endif

	void SIMDSSE::Add( float* dst, float const* src1, float const* src2, const size_t n ) const
	{
		OPFAA( dst, src1, src2, n, addps )
	}


	void SIMDSSE::Sub( float* dst, float const* src1, float const* src2, const size_t n ) const
	{
		OPFAA( dst, src1, src2, n, subps )
	}

	void SIMDSSE::Mul( float* dst, float const* src1, float const* src2, const size_t n ) const
	{
		OPFAA( dst, src1, src2, n, mulps )
	}

	void SIMDSSE::Div( float* dst, float const* src1, float const* src2, const size_t n ) const
	{
		OPFAA( dst, src1, src2, n, divps )
	}

	void SIMDSSE::Add( float* dst, float const* src1, const float value, const size_t n ) const
	{
		OPFAC1( dst, src1, value, n, addps )
	}

	void SIMDSSE::Sub( float* dst, float const* src1, const float value, const size_t n ) const
	{
		OPFAC1( dst, src1, value, n, subps )
	}

	void SIMDSSE::Mul( float* dst, float const* src1, const float value, const size_t n ) const
	{
		OPFAC1( dst, src1, value, n, mulps )
	}

	void SIMDSSE::Div( float* dst, float const* src1, const float value, const size_t n ) const
	{
		OPFAC1( dst, src1, value, n, divps )
	}

	void SIMDSSE::MulAdd( float* dst, float const* src1, const float value, const size_t n ) const
	{
		OPF2AC1( dst, src1, value, n, mulps, addps )
	}

	void SIMDSSE::MulSub( float* dst, float const* src1, const float value, const size_t n ) const
	{
		OPF2AC1( dst, src1, value, n, mulps, subps )
	}
}
