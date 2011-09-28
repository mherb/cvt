#include <cvt/io/Camera.h>
#include <cvt/io/VideoInput.h>
#include <cvt/io/ImageSequence.h>

#include "MIApp.h"

using namespace cvt;

Camera * initCamera()
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
    if( numCams == 1 ){
        selection = 0;
    } else {
        std::cout << "Select camera: ";
        std::cin >> selection;
        while ( selection >= numCams ){
            std::cout << "Index out of bounds -> select camera in Range:";
            std::cin >> selection;
        }
    }

    Camera * cam = 0;
    cam = Camera::get( selection, 640, 480, 60, IFormat::UYVY_UINT8 );
    cam->startCapture();

    return cam;
}

int main( int argc, char* argv[] )
{
    VideoInput * input = 0;
	if( argc == 1 ){
        input = initCamera();
    } else {
        String ext( "cvtraw" );
        input = new ImageSequence( argv[ 1 ], ext, 1, 2680, 5 );
    }

	try {
		MIApp app( input );
		Application::run();
	} catch( const cvt::Exception & e ) {
		std::cout << e.what() << std::endl;
	}

    if( input )
		delete input;

	return 0;
}
