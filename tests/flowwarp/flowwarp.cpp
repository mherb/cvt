#include <iostream>
#include <cvt/gfx/Image.h>
#include <cvt/io/FloFile.h>
#include <cvt/vision/Flow.h>

using namespace cvt;

int main(int argc, char* argv[])
{
	Image flow;
	Image input, output;

	if( argc < 3 ) {
	    std::cerr << "usage: " << argv[ 0 ] << " file.flo image [output]" << std::endl;
	    exit( 1 );
	}

	FloFile::FloReadFile( flow, argv[ 1 ], true );
	{
		Image tmp( argv[ 2 ] );
		tmp.convert( input, IFormat::BGRA_FLOAT );
	}
	input.warpBilinear( output, flow );

	if( argc == 4 ) {
		output.save( argv[ 3 ] );
	}

	return 0;
}
