#include <iostream>
#include <cv.h>
#include <highgui.h>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/DC1394Camera.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>

using namespace cvt;

struct Params {
	Image* src;
	Image* dst;
};

void mouseevent(int event, int x, int y, int flags, Params* p )
{
	p->dst->copyRect( 220, 140, *(p->src), x - 100, y - 100, 200, 200 );
//	cvShowImage( "Region", p->dst->iplimage() );
}

int main(int argc, char* argv[])
{	
	DC1394Camera cam;
//	V4L2Camera cam( 0, 640, 480, 30.0, CVT_BGRA );
	int key;
	size_t frames = 0;
	Time timer;


	try {
		cam.startCapture();

		Image out( 640, 480, IFormat::BGRA_UINT8 );
		Image region( 640, 480, IFormat::GRAY_UINT8 );


		Params p = { &out, &region };
		cvSetMouseCallback( "DC1394", ( CvMouseCallback ) mouseevent, &p );

		timer.reset();
		while( 1 ) {
			cam.nextFrame();
			const Image & frame = cam.frame();

			frame.convert( out );
			size_t stride;
			const uint8_t * data = out.map( &stride );
			cv::Mat ocvImage( out.height(), out.width(), CV_8UC4, (void *)data, stride );
			cv::imshow( "DC1394", ocvImage );
			frame.unmap( data );

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
		
		cam.stopCapture();
		
	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
