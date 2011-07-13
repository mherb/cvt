#include <cvt/vision/Brief.h>
#include <cvt/util/RNG.h>
#include <cvt/math/Math.h>

#include <cvt/util/CVTTest.h>
#include <cvt/io/Resources.h>

namespace cvt {
    Brief::Brief( size_t patchSize ) : _patchSize( patchSize )
    {
        //  make it odd!
        _patchSize |= 1;
        createTests();

        _blurred.reallocate( _patchSize, _patchSize, IFormat::GRAY_UINT8 );
        _tmp.reallocate( _patchSize, _patchSize, IFormat::GRAY_UINT8 );
    }

    Brief::~Brief()
    {
    }

    void Brief::descriptorForPatch( BriefDescriptor & d, const Image & patch )
    {
        // blur the patch
        patch.convolve( _tmp, IKernel::GAUSS_HORIZONTAL_7 );
        _tmp.convolve( _blurred, IKernel::GAUSS_VERTICAL_7 );

        // perform the tests
        uint8_t signature = 0;

        uint8_t * t0 = _tests;
        uint8_t * t1 = _tests + 2;

        size_t stride;
        uint8_t * p = _blurred.map( &stride );

        size_t i = 256;
        uint8_t descPart = 0;
        uint8_t descIter = 0;

        while ( i-- ) {
            if( p[ t0[ 1 ] * stride + t0[ 0 ] ] > p[ t1[ 1 ] * stride + t1[ 0 ] ] )
                signature |= 1;
            signature <<= 1;
            t0 += 4;
            t1 += 4;

            descIter++;
            if( descIter == 8 ){
                descIter = 0;
                d.set( signature, descPart );
                descPart++;
                signature = 0;
            }
        }

        _blurred.unmap( p );
    }


    void Brief::descriptorForPatch( BriefDescriptor & d, const IntegralImage & image, const Vector2f & pos )
    {
        size_t iStride;

        const float * ii = image.sumImage().map<float>( &iStride );
        size_t offset = -( _patchSize >> 1 ) - ( _patchSize >> 1 ) * iStride; // offset from patch center to upper right corner

        const float * p = ii + (int)pos.y * iStride + (int)pos.x + offset;
        const float * p0 = p;
        const float * p1 = p;

        uint8_t * t0 = _tests;
        uint8_t * t1 = _tests + 2;

        size_t i = 256;
        uint8_t descPart = 0;
        uint8_t descIter = 0;

        uint64_t signature = 0;
        size_t subWindowSize = 1;
        while ( i-- ) {
            p0 = p + t0[ 1 ] * iStride + t0[ 0 ];
            p1 = p + t1[ 1 ] * iStride + t1[ 0 ];

            /*std::cout << "FEATURE: " << pos.x << ", " << pos.y << std::endl;
            std::cout << "TO: " << ( int )t0[ 0 ] << ", " << ( int )t0[ 1 ] << std::endl;
            std::cout << "T1: " << ( int )t1[ 0 ] << ", " << ( int )t1[ 1 ] << std::endl;
            std::cout << iStride << std::endl;*/
            if( IntegralImage::area( p0, subWindowSize, subWindowSize, iStride ) > IntegralImage::area( p1, subWindowSize, subWindowSize, iStride ) )
                signature |= 1;
            signature <<= 1;

            t0 += 4;
            t1 += 4;

            descIter++;
            if( descIter == 64 ){
                descIter = 0;
                d.set( signature, descPart );
                descPart++;
                signature = 0;
            }
        }

        image.sumImage().unmap( ii );
    }

    void Brief::createTests()
    {
        RNG rng( time( NULL ) );
        double minmax = _patchSize >> 1;
        double sigma = Math::sqr( _patchSize ) / 25.0f;

        double x, y;
        for( size_t i = 0; i < 256; i++ ){
            x = Math::clamp( rng.gaussian( sigma ), -minmax, minmax) + minmax;
            y = Math::clamp( rng.gaussian( sigma ), -minmax, minmax) + minmax;
            _tests[ 4 * i ] = ( uint8_t )x;
            _tests[ 4 * i + 1 ] = ( uint8_t )y;

            x = Math::clamp( rng.gaussian( sigma ), -minmax, minmax) + minmax;
            y = Math::clamp( rng.gaussian( sigma ), -minmax, minmax) + minmax;
            _tests[ 4 * i + 2 ] = ( uint8_t )x;
            _tests[ 4 * i + 3 ] = ( uint8_t )y;
        }
    }

    BEGIN_CVTTEST( brief )

    Brief b( 7 );
    Resources resources;
    Image lena( resources.find( "lena_g.png" ) );
    Recti roi( 200, 100, 7, 7 );
    Image patch( lena, &roi );

    BriefDescriptor d;
    b.descriptorForPatch( d, patch );

    const uint8_t * desc = d.desc();

    BriefDescriptor d2( d );


    bool ret = true;
    ret &= ( d2.distance( d ) == 0 );
    uint64_t val = desc[ 0 ];
    val ^= 7;
    d2.set( val, 0 );
    ret &= ( d2.distance( d ) == 3 );

    CVTTEST_PRINT( "Hamming Distance", ret );

    return ret;

    END_CVTTEST
}


