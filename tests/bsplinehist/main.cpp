#include <cvt/gfx/Image.h>
#include <iostream>

#include "BSplineHist.h"

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 2 ) {
		std::cout << "usage: " << argv[ 0 ] << " image" << std::endl;
		return 1;
	}

	Image input, infloat;
	input.load( argv[ 1 ] );
	infloat.reallocate( input.width(), input.height(), IFormat::GRAY_FLOAT );
	input.convert( infloat );

	BSplineHist bhist( 32 );
	bhist.update( infloat );
	bhist.dump();
}
