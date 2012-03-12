#include <cvt/util/String.h>
#include <cvt/gui/Application.h>
#include "RGBDParser.h"
#include "RGBDDatasetApp.h"

using namespace cvt;

int main( int argc, char* argv[] )
{
	if( argc < 2 ){
		std::cout << "Usage: " << argv[ 0 ] << " <data_set_folder>" << std::endl;
		return 0;
	}

	try {
	String folder( argv[ 1 ] );

	RGBDDatasetApp app( folder );

	Application::run();
	} catch ( cvt::Exception& e ){
		std::cout << e.what() << std::endl;
	}

	return 0;
}
