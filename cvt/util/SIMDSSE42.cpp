#include <cvt/util/SIMDSSE42.h>

#include <xmmintrin.h>
#include <smmintrin.h>

namespace cvt
{
    size_t SIMDSSE42::hammingDistance( const uint64_t* src1, const uint64_t* src2, size_t n ) const
    {
        size_t pcount = 0;
        
        uint64_t xored;
        while ( n ) {              
            xored = *src1++^*src2++;
            pcount += _mm_popcnt_u64( xored );            
            n--;
        }
        return pcount;
    }

}
