#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <list>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/DC1394Camera.h>
#include <cvt/io/ImageIO.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>

using namespace cvt;

#define FPS ( 1.0f / 25.0f )

int main(int argc, char* argv[])
{
	const Image* frame;
	DC1394Camera cam;
//	V4L2Camera cam( 0, 640, 480, 30.0, CVT_BGRA );
	int key;
	size_t frames = 0;
	Time timer;
	size_t seq = 0;
	char buffer[ 512 ];
	Image* out;
	std::list<Image*> list;


	try {
		cam.open();
		cam.init();
		cam.captureStart();

		timer.reset();
		while( 1 ) {
			cam.captureNext();
			frame = cam.image();

			out = new Image( 640, 480, IOrder::GRAY, IType::UBYTE );
			frame->debayer( *out, IBAYER_RGGB );
//			cvShowImage( "DC1394", out->iplimage() );
			list.push_back( out );

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

	while( !list.empty() ) {
		out = list.front();
		list.pop_front();
		sprintf( buffer, "frame_%03lu.png", seq++ );
		ImageIO::savePNG( *out, buffer );
		delete out;
	}


	return 0;
}
