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

	Image output( 300, 300, IFormat::GRAY_UINT8 );
	GFXEngineImage ge( output );
	GFX g( &ge );
	g.color().set( 0, 0, 0, 255 );
	g.fillRect( 0, 0, 300, 300 );
	Eigen::VectorXf p = fshape.weights();
	for( int i = 0; i < p.rows(); i++ )
		p[ i ] = Math::rand( -0.5, 0.5f );

	g.color().set( 255, 255, 255, 255 );
	fshape.draw( &g, t, p );
//	t[ 0 ][ 0 ] += Math::rand( -2.0f, 2.0f );
//	t[ 1 ][ 1 ] = t[ 0 ][ 0 ];
//	t[ 0 ][ 2 ] += Math::rand( -5.0f, 5.0f );
//	t[ 1 ][ 2 ] += Math::rand( -5.0f, 5.0f );
	fshape.transform() = t;

//	g.color().set( 0.2f, 0.2f, 0.2f, 1.0f );
//	fshape.drawCurrent( &g );

	std::cout << std::endl;
	std::cout << fshape.weights() << std::endl;
	std::cout << std::endl;

	fshape.optimize( output, 50 );

	std::cout << p << std::endl;
	std::cout << std::endl;
	std::cout << fshape.weights() << std::endl;
	std::cout << std::endl;
	std::cout << fshape.transform() << std::endl;

	g.color().set( 128, 128, 128, 255 );
	fshape.drawCurrent( &g );

	ImageIO::savePNG( output, "meanimage.png" );
}
