#include <cvt/util/SIMDSSE42.h>

#include <xmmintrin.h>
#include <smmintrin.h>

namespace cvt
{
    /*
	size_t SIMDSSE42::hammingDistance( const uint8_t* src1, const uint8_t* src2, size_t n ) const
    {
        size_t pcount = 0;

        size_t n8 = n >> 3;
        size_t r  = n - ( n8 << 3 );

        while ( n8-- ) {
            pcount += _mm_popcnt_u64( *(uint64_t*)src1 ^ *(uint64_t*)src2 );
            src1 += 8;
            src2 += 8;
        }

        if( r ){
            uint64_t  a = 0, b = 0;

            Memcpy( ( uint8_t* ) &a, src1, r );
            Memcpy( ( uint8_t* ) &b, src2, r );

            pcount += _mm_popcnt_u64( a ^ b );
        }

        return pcount;
    }*/

}
