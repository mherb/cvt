#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>

#include <cvt/util/Time.h>

#include <cvt/vision/Flow.h>
#include <cvt/io/FloFile.h>

#include <cvt/util/Time.h>
#include <cvt/gfx/IMapScoped.h>

//#include "PDROF.h"
//#include "pmstereo.h"
//#include "gradxy.h"
//#include "weight.h"

using namespace cvt;

#define KX 16
#define KY 16
#define VIEWSAMPLES 4

int main( int argc, char** argv )
{
	if( argc != 3 ) {
		std::cout << "usage: " << argv[ 0 ] << " image1 image2" << std::endl;
		return 0;
	}

	std::vector<CLPlatform> platforms;
	std::vector<CLDevice> devs;


	CLPlatform::get( platforms );
	platforms[ 0 ].devices( devs, CL_DEVICE_TYPE_GPU );
	CL::setDefaultDevice( devs[ 0 ] );

	Image input1( argv[ 1 ] );
	Image input2( argv[ 2 ] );
	try {

	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
