#include <cvt/io/OpenNICamera.h>

#include <cvt/gui/Application.h>
#include "OpenNiWin.h"

using namespace cvt;

int main()
{
	size_t numCams = OpenNICamera::count();
	std::cout << "Num Openni cams: " << numCams << std::endl;

	/*
	CameraInfo camInfo;
	for( size_t i = 0; i < numCams; i++ ){
		OpenNICamera::cameraInfo( i, camInfo );
	}
	*/

	/*
	if( numCams == 0 )
		return 0;

	OpenNiWin win;
	Application::run();
*/
	return 0;
}
