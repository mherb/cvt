#include <cvt/io/OpenNI2Camera.h>

#include <cvt/gui/Application.h>
#include "OpenNiWin.h"

using namespace cvt;

int main()
{
	size_t numCams = OpenNI2Camera::count();
	std::cout << "Num Openni cams: " << numCams << std::endl;

	if( numCams == 0 )
		return 0;

	try {
		OpenNiWin win;
		Application::run();
	} catch ( const cvt::Exception & e ){
		std::cout << e.what() << std::endl;
	}
	
	return 0;
}
