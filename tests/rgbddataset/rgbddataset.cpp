#include <cvt/util/String.h>
#include "RGBDParser.h"

using namespace cvt;

int main( int argc, char* argv[] )
{
	if( argc < 2 ){
		std::cout << "Usage: " << argv[ 0 ] << " <data_set_folder>" << std::endl;
		return 0;
	}

	try {
	String folder( argv[ 1 ] );
	RGBDParser parser( folder );
	} catch ( cvt::Exception& e ){
		std::cout << e.what() << std::endl;
	}

	return 0;
}
