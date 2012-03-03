#include <cvt/gui/Application.h>
#include <cvt/io/Camera.h>
#include <cvt/util/Time.h>

#include "HomographyStitching.h"
#include "StitchApp.h"

using namespace cvt;

Camera* initCamera()
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

void runFromSequence( int argc, char* argv[] )
{
    if( argc < 3 ){
        std::cout << "USAGE: " << argv[ 0 ] << " <image0> <image1> <...>" << std::endl;
        return;
    }

	Matrix3f calib( 525.0f, 0.0f, 319.5f, 
				    0.0f, 525.0f, 239.5f, 
					0.0f, 0.0f, 1.0f );
	Image img;
	HomographyStitching stitcher( calib );
	
	for( int i = 1; i < argc; i++ ){
		img.load( argv[ i ] );
		Time t;
		stitcher.addImage( img );
		std::cout << "Image adding took " << t.elapsedMilliSeconds() << std::endl;
		stitcher.stitchedImage().save( "stitched.png" );
		getchar();
	}
}


void runFromCamera()
{
	Matrix3f calib( 525.0f, 0.0f, 319.5f, 
				    0.0f, 525.0f, 239.5f, 
					0.0f, 0.0f, 1.0f );

	Camera* cam = initCamera();
	StitchApp stitchApp( cam, calib );
	Application::run();

	cam->stopCapture();
	delete cam;
}

int main( int argc, char* argv[] )
{
	//runFromCamera();

	runFromSequence( argc, argv );
    return 0;
}
