#include <cvt/gfx/Image.h>
#include <cvt/io/Camera.h>
#include <cvt/io/RawVideoReader.h>
#include <cvt/gui/Application.h>

#include "KLTWindow.h"

using namespace cvt;

Camera* selectCam()
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
	if( numCams == 1 )
		selection = 0;
	else {
		std::cout << "Select camera: ";
		std::cin >> selection;
		while ( selection >= numCams ){
			std::cout << "Index out of bounds -> select camera in Range:";
			std::cin >> selection;
		}
	}

    Camera * cam = 0;

    try {
		cam = Camera::get( selection, 640, 480, 60, IFormat::UYVY_UINT8 );
	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
        return 0;
	}

	cam->startCapture();

    return cam;
}

int main( int argc, char* argv[] )
{
	VideoInput* video;
	if( argc < 2 ){
		video = selectCam();
	} else {
		video = new RawVideoReader( argv[ 1 ] );
	}

	video->nextFrame( 10 );

	KLTWindow kltWin( *video );
	Application::run();

	delete video;

	return 0;
}
