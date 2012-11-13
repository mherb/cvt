
#include <cvt/vision/ImagePyramid.h>
#include <cvt/util/CVTTest.h>
#include <cvt/io/Resources.h>
#include <cvt/gfx/IScaleFilter.h>

using namespace cvt;

static bool _filterTest( const cvt::Image& img, const cvt::IScaleFilter& filter )
{
    try {
        cvt::ImagePyramid pyr( 3, 0.5f );
        pyr.update( img, filter );
    } catch( const cvt::Exception& e ){
        return false;
    }

    return true;
}

static bool _convertTest( const cvt::Image& img )
{
    try {
        cvt::ImagePyramid pyr( 3, 0.5f );
        cvt::ImagePyramid pyrf( 3, 0.5f );
        pyr.update( img );
        pyr.convert( pyrf, IFormat::GRAY_FLOAT );
    } catch( const cvt::Exception& e ){
        return false;
    }

    return true;
}

static void _gradX( const Image& in, Image& out )
{
    out.reallocate( in );
    in.convolve( out, IKernel::HAAR_HORIZONTAL_3 );
}

static bool _funcTest( const cvt::Image& img )
{
    cvt::ImagePyramid pyr0( 3, 0.5f );
    cvt::ImagePyramid pyr1( 3, 0.5f );

    pyr0.update( img );
    pyr0.apply( pyr1, _gradX );

    return true;
}

BEGIN_CVTTEST( ImagePyramid )

cvt::Resources resources;
cvt::String file = resources.find( "lena.png" );
cvt::Image lena( file );
cvt::Image lenagf;

lena.convert( lenagf, IFormat::GRAY_FLOAT );

bool result = true;
bool b = true;

b = _filterTest( lena, cvt::IScaleFilterCubic() );
CVTTEST_PRINT( "ScaleFilterCubic", b );
result &= b;

b = _filterTest( lena, cvt::IScaleFilterBlackmanHarris( 5.0f ) );
CVTTEST_PRINT( "ScaleFilterBlackmanHarris", b );
result &= b;

b = _filterTest( lena, cvt::IScaleFilterGauss( 3.0f ) );
CVTTEST_PRINT( "ScaleFilterGauss", b );
result &= b;

b = _funcTest( lenagf );
CVTTEST_PRINT( "apply(...)", b );
result &= b;

return result;

END_CVTTEST
