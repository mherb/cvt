#ifndef CVT_BRIEF_H
#define CVT_BRIEF_H

#include <stdlib.h>
#include <stdint.h>

#include <cvt/gfx/Image.h>

namespace cvt {
    
    class BriefDescriptor
    {
    public:
        BriefDescriptor()
        {
            _desc[ 0 ] = _desc[ 1 ] = _desc[ 2 ] = _desc[ 3 ] = 0;
        }
        
        void set( uint64_t d, uint8_t i ){ _desc[ i ] = d; }
        const uint64_t * desc() const { return _desc; }
        
        uint16_t distance( const BriefDescriptor & other ) const
        {           
            // get the simd and calc the hamming distance!
            SIMD * simd = SIMD::get();
            return ( uint16_t )simd->hammingDistance( _desc, other.desc(), 4 );
        }
        
    private:
        uint64_t    _desc[ 4 ];
    };

    /* Implementation of Brief32 descriptor */    
    class Brief {
    public:
        Brief( size_t patchSize );
        ~Brief();
        
        void    descriptorForPatch( BriefDescriptor & d, const Image & patch );
                
    private:
        void createTests();
        
        size_t _patchSize;
        /* 256 tests stored ass offsets in the patch */        
        uint8_t  _tests[ 4 * 256 ];
        
        Image   _blurred, _tmp;
        
    };
}

#endif