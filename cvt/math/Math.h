#ifndef CVTMATH_H
#define CVTMATH_H
#include <stdlib.h>

namespace cvt {

    namespace Math
    {
	template<class T> static inline T max( T x, T y ) { return ( x > y ) ? x : y; }
	template<class T> static inline T min( T x, T y ) { return ( x < y ) ? x : y; }
	template<class T> static inline size_t maxIndex( T x, T y ) { return  ( x > y ) ? 0 : 1; }
	template<class T> static inline size_t minIndex( T x, T y ) { return ( x < y ) ? 0 : 1; }

	template<typename T> static inline T mix( T a, T b, T alpha ) { return a + alpha * ( b - a ); }
	template<typename T> static inline T clamp( T v, T min, T max ) { return ( v < min ) ? min : ( v > max ) ? max : v; }

	static inline size_t pad16( size_t v ) { return v & 0xf ?  ( ( v | 0xf ) + 1 ) : v; }
    }
}


#endif
