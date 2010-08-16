#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <cvt/gfx/Image.h>
#include <cvt/io/V4L2Camera.h>
#include <cvt/util/Timer.h>

using namespace cvt;

int main(int argc, char* argv[])
{
	const Image* frame;
	V4L2Camera cam( 0, 640, 480, 30.0, CVT_GRAY );
	int key;
	size_t frames = 0;
	Timer timer;
	Image f, dx, dy;


	cam.open();
	cam.init();
	cam.captureStart();

	timer.reset();
	while( 1 ) {
	    cam.captureNext();
	    frame = cam.image();

	    frame->convert( f, CVT_GRAY, CVT_FLOAT );
	    f.dx( dx );
	    f.dy( dy );
	    cvShowImage( "V4L2", dx.iplimage() );
	    cvShowImage( "V4L2-2", dy.iplimage() );

	    key = cvWaitKey( 10 );
	    if( ( key & 0xff ) == 27 )
		break;
	    frames++;
	    if( timer.elapsedSeconds() > 5.0f ) {
		std::cout << "FPS: " << ( double ) frames / timer.elapsedSeconds() << std::endl;
		frames = 0;
		timer.reset();
	    }
	}

	return 0;
}
