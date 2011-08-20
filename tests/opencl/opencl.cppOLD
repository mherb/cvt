#include <cvt/cl/CLContext.h>
#include "CLOptflow.h"
#include <cvt/gfx/Image.h>
#include <cvt/gfx/Color.h>
#include <cvt/util/Exception.h>
#include <cvt/util/Time.h>
#include <cvt/io/FloFile.h>
#include <cvt/vision/Flow.h>
#include <string>
#include <iostream>

#include <cvt/io/Camera.h>

#include "calcp1.h"
#include "calcp2.h"
#include "clearimg.h"
#include "copyimg.h"
#include "bilindown.h"
#include "warp.h"
#include "warpsub.h"
#include "grad.h"

using namespace cvt;

int main()
{
	Image frame( 640, 480, IFormat::GRAY_UINT8 );
	Camera* cam;
	std::string log;
	bool doprocess = true;
	size_t frames = 0;
	Time timer;
	std::vector<cl::Event> sync;
	cl::Event event;
	Image* u;

	Camera::updateInfo();
	cam = Camera::get( 0, 640, 480, 60.0, IFormat::YUYV_UINT8 );

	try {
		CLContext cl;
		cl.makeCurrent();

		cl.info( std::cout );
		CLOptflow flow;


//		Image iflow( 640, 480, IFormat::RGBA_UINT8 );

		timer.reset();

		cam->startCapture();

		while( 1 ) {
			cam->nextFrame();
			cam->frame().convert( frame );
			if( doprocess ) {
				u = flow.updateFlow( frame );
				//u->readData( iflow.data(), iflow.stride() );

/*				size_t stride;
				const uint8_t * data = u->map( &stride );
				cv::Mat ocvImage( (int)u->height(), (int)u->width(), CV_8UC4, (void *)data, stride );
				cv::imshow( "frame", ocvImage );
				u->unmap( data );*/

				delete u;

//				cvShowImage( "Video", iflow.iplimage() );
			} else {
/*				size_t stride;
				const uint8_t * data = frame.map( &stride );
				cv::Mat ocvImage( (int)frame.height(), (int)frame.width(), CV_8UC1, (void *)data, stride );
				cv::imshow( "frame", ocvImage );
				frame.unmap( data );*/
			}
/*			key = cvWaitKey( 5 ) & 0xff;
			if( key == 27 )
				break;
			else if( key == ' ')
				doprocess = !doprocess;*/

			frames++;
			if( timer.elapsedSeconds() > 5.0f ) {
				std::cout << "FPS: " << ( double ) frames / timer.elapsedSeconds() << std::endl;
				frames = 0;
				timer.reset();
			}

		}

		cam->stopCapture();
	} catch( CLException e ) {
		std::cout << e.what() << std::endl;
	} catch( Exception e ) {
		std::cout << e.what() << std::endl;
	}
}
