#include "FaceShape.h"
#include <cvt/gfx/Image.h>
#include <cvt/gfx/GFX.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/io/ImageIO.h>

#include <iostream>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 2 ) {
		std::cout << argv[ 0 ] << " shapefile" << std::endl;
		return 1;
	}

	FaceShape fshape;
	fshape.load( argv[ 1 ] );

	Matrix3f t;
	t.identity();
	t[ 0 ][ 0 ] = 35;
	t[ 1 ][ 1 ] = 35;
	t[ 0 ][ 2 ] = 140;
	t[ 1 ][ 2 ] = 160;

	Image output;//( 300, 300, IFormat::BGRA_UINT8 );
	ImageIO::loadPNG( output, "/home/heise/Pictures/myface.png" );
	GFXEngineImage ge( output );
	GFX g( &ge );
//	g.color().set( 0, 0, 0, 255 );
//	g.fillRect( 0, 0, 300, 300 );
	fshape.drawMean( &g, t );
	ImageIO::savePNG( output, "meanimage.png" );
}
