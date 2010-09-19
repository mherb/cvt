#include <cvt/cl/CLContext.h>
#include "CLOptflow.h"
#include <cvt/gfx/Image.h>
#include <cvt/gfx/Color.h>
#include <cvt/gfx/IFilterScalar.h>
#include <cvt/io/ImageIO.h>
#include <cvt/util/Exception.h>
#include <cvt/util/Timer.h>
#include <cvt/io/FloFile.h>
#include <cvt/vision/Flow.h>
#include <string>
#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <cvt/io/V4L2Camera.h>
#include <cvt/io/DC1394Camera.h>

#include "calcp1.h"
#include "calcp2.h"
#include "clearimg.h"
#include "copyimg.h"
#include "bilindown.h"
#include "warp.h"
#include "warpsub.h"
#include "grad.h"

using namespace cvt;

int main( int argc, char** argv )
{
	const Image* frame;
//	V4L2Camera cam( 0, 640, 480, 30.0, CVT_GRAY );
	DC1394Camera cam;
	std::string log;
	bool doprocess = true;
	int key;
	size_t frames = 0;
	Timer timer;
	std::vector<cl::Event> sync;
	cl::Event event;
	int pyridx = 0;
	CLImage* u;

	try {
		CLContext cl;
		cl.makeCurrent();
		CLOptflow flow;


		Image iflow( 640, 480, IOrder::RGBA, IType::UBYTE );

		cam.open();
		cam.init();
		cam.captureStart();
		timer.reset();

		while( 1 ) {
			cam.captureNext();
			frame = cam.image();
			if( doprocess ) {
				int pyridx2 = pyridx ^ 0x01;
				flow.updateFlow( *frame );
			} else
				cvShowImage( "Video", frame->iplimage() );

			key = cvWaitKey( 10 ) & 0xff;
			if( key == 27 )
				break;
			else if( key == ' ')
				doprocess = !doprocess;

			frames++;
			if( timer.elapsedSeconds() > 5.0f ) {
				std::cout << "FPS: " << ( double ) frames / timer.elapsedSeconds() << std::endl;
				frames = 0;
				timer.reset();
			}

		}
	} catch( CLException e ) {
		std::cout << e.what() << std::endl;
	} catch( Exception e ) {
		std::cout << e.what() << std::endl;
	}
}
