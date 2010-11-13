#include <cvt/io/Camera.h>

using namespace cvt;

int main( int argc, char* argv[] )
{
	Camera::updateInfo();
	
	size_t numCams = Camera::count();
	
	std::cout << "Overall number of Cameras: " << numCams << std::endl;
	
	for( size_t i = 0; i < numCams; i++ ){
		const CameraInfo & info = Camera::info( i );
		
		std::cout << "Camera " << i << std::endl;
		std::cout << "Name: " << info.name() << std::endl;
	}
	
	return 0;
}