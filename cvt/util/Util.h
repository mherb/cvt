#ifndef CVT_UTIL_H
#define CVT_UTIL_H

#include <stdint.h>

namespace cvt {

	class Util {
		public:
			static inline uint16_t bswap16( uint16_t x );
			static inline uint32_t bswap32( uint32_t x );
			static inline uint64_t bswap64( uint64_t x );
	};

	inline uint16_t Util::bswap16( uint16_t x )
	{
		return ( x << 8 ) | ( x >> 8 );
	}

	inline uint32_t Util::bswap32( uint32_t x )
	{
		return ( x << 24) | ( x >> 24 ) | ( ( x << 8 ) & 0xff0000 ) | ( ( x >> 8 ) & 0xff00 );
	}

	inline uint64_t Util::bswap64( uint64_t x )
	{
		return ( ( uint64_t ) bswap32( x & 0xffffffff ) ) << 32 | ( uint64_t ) bswap32( x >> 32 );
	}
}

#endif
