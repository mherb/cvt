#include <iostream>
#include <cvt/gfx/Image.h>
#include <cvt/io/FloFile.h>
#include <cvt/io/ImageIO.h>
#include <cvt/vision/Flow.h>

#include <cv.h>
#include <highgui.h>

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
		tmp.convert( input, IOrder::BGRA, IType::FLOAT );
	}
	input.warpBilinear( output, flow );


	while( 1 ) {
		cvShowImage( "Flow", output.iplimage() );

		key = cvWaitKey( 10 ) & 0xff;
		if( key == 27 )
			break;
	}

	if( argc == 4 ) {
		ImageIO::savePNG( output, argv[ 3 ] );
	}

	return 0;
}
