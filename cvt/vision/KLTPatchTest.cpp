#include <cvt/vision/KLTPatch.h>
#include <cvt/util/CVTTest.h>
#include <cvt/io/Resources.h>

#include <cvt/math/GA2.h>
#include <cvt/math/SL3.h>
#include <cvt/math/Translation2D.h>

using namespace cvt;


static bool _dumpPatch( const ImagePyramid& gray,
                        const ImagePyramid& gx,
                        const ImagePyramid& gy )
{
    typedef Translation2D<float> PoseType;
    typedef KLTPatch<20, PoseType> Patch;

    Patch p( gray.octaves() );
    p.update( gray, gx, gy, Vector2f( 266, 266 ) );

    Image img;
    String name;
    for( size_t i = 0; i < gray.octaves(); i++ ){
        p.toImage( img, i );
        name.sprintf( "patch_octave_%02d.png", i );
        img.save( name );
    }

    return true;
}

template<class PoseType>
static bool _trackPatch( const ImagePyramid& gray,
                         const ImagePyramid& gx,
                         const ImagePyramid& gy )
{
    typedef KLTPatch<20, PoseType> Patch;

    Vector2f truePos( 266, 266 ), estimated;

    Patch p( gray.octaves() );
    if( !p.update( gray, gx, gy, truePos ) ){
        std::cout << "UPDATE AT POSITION FAILED!" << std::endl;
    }

    Vector2f initial( truePos.x + 10, truePos.y - 10 );
    p.initPose( initial );
    p.currentCenter( initial );
    p.align( gray );
    p.currentCenter( estimated );

    if( ( truePos - estimated ).length() > 1e-1 ){
        std::cout << "Initial: " << initial;
        std::cout << " - True: " << truePos;
        std::cout << " -> Estimated: " << estimated << std::endl;
        return false;
    }

    return true;
}

BEGIN_CVTTEST( KLTTracker )

Resources resources;
String file = resources.find( "lena.png" );
Image lena( file );
Image lenagf;
lena.convert( lenagf, IFormat::GRAY_FLOAT );

ImagePyramid pyrf( 3, 0.5f );
ImagePyramid gx( 3, 0.5f );
ImagePyramid gy( 3, 0.5f );
pyrf.update( lenagf );
IKernel kx( IKernel::HAAR_HORIZONTAL_3 );
IKernel ky( IKernel::HAAR_VERTICAL_3 );
kx.scale( -0.5f );
ky.scale( -0.5f );

pyrf.convolve( gx, kx );
pyrf.convolve( gy, ky );


bool result = true;
bool b = true;

b = _dumpPatch( pyrf, gx, gy );
CVTTEST_PRINT( "Patch Dumping", b );
result &= b;

b = _trackPatch<Translation2D<float> >( pyrf, gx, gy );
CVTTEST_PRINT( "2D Translation Test: ", b );
result &= b;

b = _trackPatch<GA2<float> >( pyrf, gx, gy );
CVTTEST_PRINT( "2D General Affine Test: ", b );
result &= b;

return result;

END_CVTTEST
