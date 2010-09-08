#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLImage.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/Color.h>
#include <cvt/io/ImageIO.h>
#include <cvt/util/Exception.h>
#include <cvt/util/Timer.h>
#include <string>
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cvt/io/V4L2Camera.h>

#include "calcp1.h"
#include "calcp2.h"
#include "clearimg.h"
#include "copyimg.h"

using namespace cvt;

int main()
{
	const Image* frame;
	V4L2Camera cam( 0, 640, 480, 30.0, CVT_GRAY );
	std::string log;
	bool doprocess = true;
	int key;
	size_t frames = 0;
	Timer timer;

	CLContext cl;
	cl.makeCurrent();

	CLKernel kernelp1, kernelp2, kernelcopyimg, kernelclearimg;

	kernelcopyimg.build( "Denoise_COPYIMG", _copyimg_source, strlen( _copyimg_source ), log );
	std::cout << "Log: " << log << std::endl;
	kernelclearimg.build( "Denoise_CLEARIMG", _clearimg_source, strlen( _clearimg_source ), log );
	std::cout << "Log: " << log << std::endl;
	kernelp1.build( "Denoise_CALCP1", _calcp1_source, strlen( _calcp1_source ), log );
	std::cout << "Log: " << log << std::endl;
	kernelp2 .build( "Denoise_CALCP2", _calcp2_source, strlen( _calcp2_source ), log );
	std::cout << "Log: " << log << std::endl;

	cam.open();
	cam.init();
	cam.captureStart();
	timer.reset();

	while( 1 ) {
		cam.captureNext();
		frame = cam.image();
		if( doprocess )
			cvShowImage( "Video", frame->iplimage() );
		else
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

	/*	CLImage image( 640, 480 );
		Color c( 1.0f, 0.0f, 0.0f, 1.0f );
		kernel.setArg( 0 , &image );
		kernel.setArg( 1 , &c );
		kernel.run( ::cl::NullRange, image.globalRange(), ::cl::NDRange( 16, 16 ) );*/
}
