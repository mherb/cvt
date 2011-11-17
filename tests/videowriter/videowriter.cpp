#include <cvt/io/RawVideoWriter.h>
#include <cvt/io/Camera.h>
#include <cvt/util/Time.h>

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

    return cam;
}

int main()
{

	Camera * cam = selectCam();
	
	cam->startCapture();
	cam->nextFrame();

	try {
		RawVideoWriter writer( "test.rawvid" );

		Time t;
		while( t.elapsedSeconds() < 20 ){
			cam->nextFrame();
			writer.write( cam->frame() );
		}
		std::cout << "Done" << std::endl;

	} catch ( const Exception & e ){
		std::cout << e.what() << std::endl;
	}

	cam->stopCapture();
	delete cam;

	return 0;
}
