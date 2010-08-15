#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <cvt/gfx/Image.h>
#include <cvt/io/V4L2Camera.h>

using namespace cvt;

int main(int argc, char* argv[])
{
	const Image* frame;
	V4L2Camera cam( 0, 640, 480, 30.0 );
	int key;


	cam.open();
	cam.init();
	cam.captureStart();

	while( 1 ) {
	    cam.captureNext();
	    frame = cam.image();

	    cvShowImage( "V4L2", frame->iplimage() );

	    key = cvWaitKey( 10 );
	    if( ( key & 0xff ) == 27 )
		break;
	}

	return 0;
}
