#include <iostream>
#include <cv.h>
#include <highgui.h>

#include <cvt/gfx/Image.h>
#include <cvt/util/Timer.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>
#include <cvt/util/Exception.h>
#include <cvt/vision/Flow.h>
#include "calcflow.h"

using namespace cvt;

int main(int argc, char* argv[])
{
	int key;
	Image img1,img2;
	Image in1, in2;
	Image flow, cflow;
	ROFDenoise denoise;

	if( argc != 3 )
		return 1;

	ImageIO::loadPNG( img1, argv[ 1 ] );
	ImageIO::loadPNG( img2, argv[ 2 ] );

	Image* tmp = new Image();
#if 1
	img1.convert( *tmp, CVT_GRAY, CVT_FLOAT );
	denoise.apply( in1, *tmp, 0.125f, 100 );
	in1.mad( *tmp, -0.95f );
	in1.mul(10.0f);
	img2.convert( *tmp, CVT_GRAY, CVT_FLOAT );
	denoise.apply( in2, *tmp, 0.125f, 100 );
	in2.mad( *tmp, -0.95f );
	in2.mul(10.0f);
#else
	img1.convert( in1, CVT_GRAY, CVT_FLOAT );
	img2.convert( in2, CVT_GRAY, CVT_FLOAT );
#endif
	delete tmp;

	cvShowImage( "Frame 1", in1.iplimage() );
	cvShowImage( "Frame 2", in2.iplimage() );

	calcflow( flow, in1, in2 );
	Flow::colorCode( cflow, flow );

	while( 1 ) {
		cvShowImage( "Flow", cflow.iplimage() );
		key = cvWaitKey( 10 ) & 0xff;
		if( key == 27 )
			break;
	}
	return 0;
}
