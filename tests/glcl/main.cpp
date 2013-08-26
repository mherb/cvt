#include <cvt/gui/Application.h>
#include <cvt/util/Exception.h>
#include <cvt/cl/OpenCL.h>

#include <iostream>


using namespace cvt;


int main()
{
	try {
		Application::init();
		std::cout << "GL:" << Application::hasGLSupport() << std::endl;
		std::cout << "CL:" << Application::hasCLSupport() << std::endl;
		std::cout << "CL/GL:" << CL::defaultGLsharing() << std::endl;
	} catch( Exception& e ) {
		std::cout << e.what() << std::endl;
	}
}
