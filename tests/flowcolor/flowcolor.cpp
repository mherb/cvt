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
	Image color;

	if( argc < 2 ) {
	    std::cerr << "usage: " << argv[ 0 ] << "file.flo [out.png]" << std::endl;
	    exit( 1 );
	}

	FloFile::FloReadFile( flow, argv[ 1 ] );
	Flow::colorCode( color, flow );


	while( 1 ) {
		cvShowImage( "Flow", color.iplimage() );

		key = cvWaitKey( 10 ) & 0xff;
		if( key == 27 )
			break;
	}

	if( argc == 3 ) {
		ImageIO::savePNG( color, argv[ 2 ] );
	}

	return 0;
}
