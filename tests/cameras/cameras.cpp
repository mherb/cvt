#include <cvt/io/Camera.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>

#include <cv.h>
#include <highgui.h>

using namespace cvt;

int main()
{
	Camera::updateInfo();
	size_t numCams = Camera::count();

	std::cout << "Overall number of Cameras: " << numCams << std::endl;
	if( numCams == 0 ){
		std::cout << "Please connect a camera!" << std::endl;
		return 0;
	}

	for( size_t i = 0; i < numCams; i++ ){
		const CameraInfo & info = Camera::info( i );
		std::cout << "Camera " << i << ": " << info << std::endl;
	}

	size_t selection = numCams;
	std::cout << "Select camera: ";
	std::cin >> selection;
	while ( selection >= numCams ){
		std::cout << "Index out of bounds -> select camera in Range:";
		std::cin >> selection;
	}


	int key;
	size_t frames = 0;
	Time timer;
	Camera * cam = 0;

	try {
		cam = Camera::get( selection, 640, 480, 30, IFormat::YUYV_UINT8 );

		Image frame( cam->width(), cam->height(), IFormat::BGRA_UINT8 );

		cam->startCapture();

		timer.reset();
		while( 1 ) {
			cam->nextFrame();
			cam->frame().convert( frame );

			size_t stride;
			const uint8_t * data = frame.map( &stride );
			cv::Mat ocvImage( (int)frame.height(), (int)frame.width(), CV_8UC4, (void *)data, stride );
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

		cam->stopCapture();
	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}

	if(cam)
		delete cam;

	return 0;
}
