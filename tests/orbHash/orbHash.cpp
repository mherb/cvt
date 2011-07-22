#include <cvt/gfx/Image.h>
#include <cvt/gfx/GFX.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/vision/ORB.h>
#include <cvt/io/Resources.h>

#include "LSH.h"

using namespace cvt;

int main( int argc, char * argv[] )
{
	Resources res;

	Image img, c;
	Image img2;

	c.load( res.find( "features_dataset/boat/img1.ppm" ) );
    c.convert( img, IFormat::GRAY_UINT8 );
//	img2.load( res.find( "lena_g_scale.png" ).c_str() );
	img2.load( res.find( "lena_g_rot.png" ).c_str() );
//	img2.load( res.find( "lena_g.png" ).c_str() );

	Image out( img.width(), img.height(), IFormat::RGBA_UINT8 );
	Image out2( img2.width(), img2.height(), IFormat::RGBA_UINT8 );
	img.convert( out );
	img2.convert( out2 );

	Recti imgRect( 0, 0, img.width(), img.height() );
	Image correspondences( img2.width() * 2, img2.height(), IFormat::RGBA_UINT8 );
	correspondences.copyRect( 0, 0, out, imgRect );
	correspondences.copyRect( img.width(), 0, out2, imgRect );

	ORB orb1( img, 3, 0.5, 40, 0 );

    LSH<10> lsh( orb1 );

	ORB orb2( img2, 3, 0.5, 40, 0 );


	return 0;
}
