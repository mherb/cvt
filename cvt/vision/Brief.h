#ifndef CVT_BRIEF_H
#define CVT_BRIEF_H

#include <stdlib.h>
#include <stdint.h>

#include <cvt/vision/IntegralImage.h>
#include <cvt/gfx/Image.h>

namespace cvt {

    class BriefDescriptor
    {
    public:
        BriefDescriptor()
        {
            memset( _desc, 0, 32 );
        }

        BriefDescriptor( const BriefDescriptor & other )
        {
            SIMD * simd = SIMD::instance();
            simd->Memcpy( _desc, other.desc(), 32 );
        }

        void set( uint8_t d, size_t i ){ _desc[ i ] = d; }
        const uint8_t * desc() const { return _desc; }

        uint16_t distance( const BriefDescriptor & other ) const
        {
            // get the simd and calc the hamming distance!
            SIMD * simd = SIMD::instance();
            return ( uint16_t )simd->hammingDistance( _desc, other.desc(), 4 );
        }

    private:
        uint8_t    _desc[ 32 ];
    };

    /* Implementation of Brief32 descriptor */
    class Brief {
    public:
        Brief( size_t patchSize );
        ~Brief();

        /* perform normal tests within given patch */
        void    descriptorForPatch( BriefDescriptor & d, const Image & patch );

        /* use integral image for testing and test: area( t1 ) > area ( t2 ) */
        void    descriptorForPatch( BriefDescriptor & d, const IntegralImage & image, const Vector2f & pos );

    private:
        void createTests();

        size_t _patchSize;
        /* 256 tests stored ass offsets in the patch */
        uint8_t  _tests[ 4 * 256 ];

        Image   _blurred, _tmp;

    };
}

#endif