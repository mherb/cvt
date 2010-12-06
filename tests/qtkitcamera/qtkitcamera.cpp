#include <iostream>
#include <cv.h>
#include <highgui.h>

#include <cvt/gfx/Image.h>
#include <cvt/io/QTKitCamera.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>

using namespace cvt;

int main(int argc, char* argv[])
{
	unsigned int numCams = QTKitCamera::count();

	std::cout << "Available QTKit Devices: " << numCams << std::endl;
	if( numCams == 0 )
		return 0;
	CameraInfo info;
	QTKitCamera::QTKitCamera::cameraInfo( 0, info );

	QTKitCamera cam( 0, info.bestMatchingMode(IFormat::BGRA_UINT8, 800, 600, 30) );

	Image gray( cam.width(), cam.height(), IFormat::GRAY_UINT8 );

	int key;
	size_t frames = 0;
	Time timer;


	try {
		timer.reset();

		while( 1 ) {
			cam.nextFrame();
			const Image & frame = cam.frame();

			frame.convert( gray );

			size_t stride;
			const uint8_t * data = gray.map( &stride );
			cv::Mat ocvImage( gray.height(), gray.width(), CV_8UC1, (void *)data, stride );
			cv::imshow( "frame", ocvImage );
			gray.unmap( data );

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
