#include <cvt/gfx/Image.h>
#include <iostream>

#include <cvt/gfx/IHistogram.h>

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

	IHistogramf bhist( 10 );
	bhist.update( infloat );
	for( size_t i = 0; i < 10; i++)
		std::cout << i << " " << bhist( i ) << std::endl;
}
