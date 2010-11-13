#include <iostream>
#include <cv.h>
#include <highgui.h>

#include <cvt/gfx/Image.h>
#include <cvt/io/QTKitCamera.h>
#include <cvt/util/Timer.h>
#include <cvt/util/Exception.h>

using namespace cvt;

int main(int argc, char* argv[])
{
	unsigned int numCams = QTKitCamera::numberOfCameras();
	
	std::cout << "Available QTKit Devices: " << numCams << std::endl;
	
	QTKitCamera cam( 0 );

	int key;
	size_t frames = 0;
	Timer timer;


	try {		
		timer.reset();
		while( 1 ) {
			cam.nextFrame();
			const Image & frame = cam.frame();
			
			size_t stride;
			const uint8_t * data = frame.map( &stride );
			cv::Mat ocvImage( frame.height(), frame.width(), CV_8UC4, (void *)data, stride );			
			cv::imshow( "frame", ocvImage );
			frame.unmap( data );
			
			key = cvWaitKey( 3 ) & 0xff;
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
