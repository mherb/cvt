#ifndef CPU_H
#define CPU_H
#include <iostream>
#include <stdint.h>

namespace cvt {
	enum CPUFeature {
		CPU_BASE   =  0,
		CPU_MMX    = ( 1 << 1 ),
		CPU_SSE    = ( 1 << 2 ),
		CPU_SSE2   = ( 1 << 3 ),
		CPU_SSE3   = ( 1 << 4 ),
		CPU_SSSE3  = ( 1 << 5 ),
		CPU_SSE4_1 = ( 1 << 6 ),
		CPU_SSE4_2 = ( 1 << 7 ),
		CPU_AVX    = ( 1 << 8 ),
	};

	static inline uint32_t cpuFeatures( void )
	{
		uint32_t ret = CPU_BASE;
		uint32_t eax, ebx, ecx, edx;

		asm("\tmovl %%ebx, %%esi;\n\t"
			"movl $1, %%eax;\n\t"
			"cpuid;\n\t"
			"xchgl %%esi, %%ebx;\n\t"
				: "=a"(eax), "=S"(ebx), "=c"(ecx), "=d"(edx)
			);
		if( edx & ( 1 << 23 ) )
			ret |= CPU_MMX;
		if( edx & ( 1 << 25 ) )
			ret |= CPU_SSE;
		if( edx & ( 1 << 26 ) )
			ret |= CPU_SSE2;
		if( ecx & ( 1 <<  0 ) )
			ret |= CPU_SSE3;
		if( ecx & ( 1 <<  0 ) )
			ret |= CPU_SSE3;
		if( ecx & ( 1 <<  9 ) )
			ret |= CPU_SSSE3;
		if( ecx & ( 1 << 19 ) )
			ret |= CPU_SSE4_1;
		if( ecx & ( 1 << 20 ) )
			ret |= CPU_SSE4_2;
		if( ecx & ( 1 << 28 ) )
			ret |= CPU_AVX;
		return ret;
	}

	static inline void cpuFeaturesPrint()
	{
		uint32_t f;
		f = cpuFeatures();
		std::cout << "CPU features: ";
		if( f & CPU_MMX )
			std::cout << "MMX ";
		if( f & CPU_SSE )
			std::cout << "SSE ";
		if( f & CPU_SSE2 )
			std::cout << "SSE2 ";
		if( f & CPU_SSE3 )
			std::cout << "SSE3 ";
		if( f & CPU_SSSE3 )
			std::cout << "SSSE3 ";
		if( f & CPU_SSE4_1 )
			std::cout << "SSE4.1 ";
		if( f & CPU_SSE4_2 )
			std::cout << "SSE4.2 ";
		if( f & CPU_AVX )
			std::cout << "AVX ";
		std::cout << std::endl;
	}
}

#endif
