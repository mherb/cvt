#include <iostream>
#include <cvt/gfx/Image.h>
#include <cvt/io/FloFile.h>
#include <cvt/io/ImageIO.h>
#include <cvt/vision/Flow.h>

using namespace cvt;

int main(int argc, char* argv[])
{
	int key;
	Image flow;
	Image input, output;

	if( argc < 3 ) {
	    std::cerr << "usage: " << argv[ 0 ] << " file.flo image [output]" << std::endl;
	    exit( 1 );
	}

	FloFile::FloReadFile( flow, argv[ 1 ], true );
	{
		Image tmp;
		ImageIO::loadPNG( tmp, argv[ 2 ] );
		tmp.convert( input, IFormat::BGRA_FLOAT );
	}
	input.warpBilinear( output, flow );

	if( argc == 4 ) {
		ImageIO::savePNG( output, argv[ 3 ] );
	}

	return 0;
}
