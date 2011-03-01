#include "FaceShape.h"
#include <cvt/gfx/Image.h>
#include <cvt/math/Math.h>
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

	srand( time( NULL ) );

	FaceShape fshape;
	fshape.load( argv[ 1 ] );

	Matrix3f t;
	t.identity();
	t[ 0 ][ 0 ] = 45;
	t[ 1 ][ 1 ] = 45;
	t[ 0 ][ 2 ] = 140;
	t[ 1 ][ 2 ] = 160;

	Image output( 300, 300, IFormat::BGRA_UINT8 );
	GFXEngineImage ge( output );
	GFX g( &ge );
	g.color().set( 0, 0, 0, 255 );
	g.fillRect( 0, 0, 300, 300 );
	Eigen::VectorXf p = fshape.weights();
	for( int i = 0; i < p.rows(); i++ )
		p[ i ] = Math::rand( -0.5, 0.5f );
	fshape.draw( &g, t, p );
	fshape.transform() = t;

	ImageIO::savePNG( output, "meanimage.png" );
}
