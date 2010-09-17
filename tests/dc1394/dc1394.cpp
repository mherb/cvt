#include <iostream>
#include <cv.h>
#include <highgui.h>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/DC1394Camera.h>
#include <cvt/util/Timer.h>
#include <cvt/util/Exception.h>

using namespace cvt;


int main(int argc, char* argv[])
{
	const Image* frame;
	DC1394Camera cam;
//	V4L2Camera cam( 0, 640, 480, 30.0, CVT_BGRA );
	int key;
	size_t frames = 0;
	Timer timer;


	try {
		cam.open();
		cam.init();
		cam.captureStart();

		Image out( 640, 480, CVT_RGBA, CVT_UBYTE );

		timer.reset();
		while( 1 ) {
			cam.captureNext();
			frame = cam.image();

			frame->debayer( out, IBAYER_RGGB );
			cvShowImage( "DC1394", out.iplimage() );

			key = cvWaitKey( 10 ) & 0xff;
			if( key == 27 )
				break;

			frames++;
			if( timer.elapsedSeconds() > 5.0f ) {
				std::cout << "FPS: " << ( double ) frames / timer.elapsedSeconds() << std::endl;
				frames = 0;
				timer.reset();
			}
		}
	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
