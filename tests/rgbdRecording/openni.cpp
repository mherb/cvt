#include <cvt/io/OpenNICamera.h>

#include <cvt/gui/Application.h>


#include "OpenNiWin.h"

using namespace cvt;

int main()
{
	size_t numCams = OpenNICamera::count();
	std::cout << "Num Openni cams: " << numCams << std::endl;

	ConfigFile cfg( "config.cfg" );

	if( numCams == 0 )
		return 0;

	OpenNiRecorder recorder( cfg );
	recorder.run();
	
	return 0;
}
