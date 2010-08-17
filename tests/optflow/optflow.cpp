#include <iostream>
#include <cv.h>
#include <highgui.h>

#include <cvt/gfx/Image.h>
#include <cvt/util/Timer.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>

using namespace cvt;

int main(int argc, char* argv[])
{
	int key;
	Image img1,img2;
	Image in1, in2;
	ROFDenoise denoise;

	if( argc != 3 )
	    return 1;

	ImageIO::loadPNG( img1, argv[ 1 ] );
	ImageIO::loadPNG( img2, argv[ 2 ] );

	Image* tmp = new Image();
	img1.convert( *tmp, CVT_GRAY, CVT_FLOAT );
	denoise.apply( in1, *tmp, 0.125f, 100 );
	in1.mad( *tmp, -0.95f );
	img2.convert( *tmp, CVT_GRAY, CVT_FLOAT );
	denoise.apply( in2, *tmp, 0.125f, 100 );
	in2.mad( *tmp, -0.95f );
	delete tmp;

	while( 1 ) {

	    cvShowImage( "V4L2 - Frame 1", img1.iplimage() );
	    cvShowImage( "V4L2 - Frame 2", img2.iplimage() );
	    cvShowImage( "V4L2 - Frame 1 Structure", in1.iplimage() );
	    cvShowImage( "V4L2 - Frame 2 Structure", in2.iplimage() );

	    key = cvWaitKey( 10 ) & 0xff;
	    if( key == 27 )
		break;
	}

	return 0;
}
