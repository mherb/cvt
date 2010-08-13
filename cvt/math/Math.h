#ifndef CVTMATH_H
#define CVTMATH_H
#include <stdlib.h>

namespace cvt {
    template<class T> static inline T Max( T x, T y ) { return ( x > y ) ? x : y; }
    template<class T> static inline T Min( T x, T y ) { return ( x < y ) ? x : y; }
    template<class T> static inline size_t MaxIndex( T x, T y ) { return  ( x > y ) ? 0 : 1; }
    template<class T> static inline size_t MinIndex( T x, T y ) { return ( x < y ) ? 0 : 1; }

    namespace Math
    {
	    template<typename T> static inline T mix( T a, T b, T alpha ) { return a + alpha * ( b - a ); }
	    template<typename T> static inline T clamp( T v, T min, T max ) { return ( v < min ) ? min : ( v > max ) ? max : v; }

	    static inline size_t pad16( size_t v ) { return v & 0xf ?  ( ( v | 0xf ) + 1 ) : v; }
    }
}


#endif
