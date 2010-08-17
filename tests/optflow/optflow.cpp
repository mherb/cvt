#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <cvt/gfx/Image.h>
#include <cvt/util/Timer.h>
#include <cvt/io/ImageIO.h>
#include <cvt/gfx/ifilter/ROFDenoise.h>

using namespace cvt;

int main(int argc, char* argv[])
{
	int key;
	Image img, out, itmp;
	ROFDenoise denoise;
	bool dofilter = true;

	ImageIO::loadPNG( img, argv[ 1 ] );

	out = img;
	img.convert( itmp, CVT_GRAY, CVT_FLOAT );
/*	out.reallocate( itmp );
	cvSmooth( itmp.iplimage(), out.iplimage(), CV_MEDIAN, 3, 0, 0 );*/
	denoise.apply( out, itmp, 0.125f, 100 );
	itmp.mad( out, -0.95f );
//	out = itmp;
	out.mad( itmp, 2.0f );

	while( 1 ) {

	    if( dofilter ) {
		cvShowImage( "V4L2", out.iplimage() );
	    } else
		cvShowImage( "V4L2", img.iplimage() );

	    key = cvWaitKey( 10 );
	    if( ( key & 0xff ) == 27 )
		break;
	    else if( ( key & 0xff ) == ' ' )
		dofilter = !dofilter;

	}

	return 0;
}
