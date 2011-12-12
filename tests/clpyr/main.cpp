#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>

#include <cvt/vision/Flow.h>

#include <cvt/util/Time.h>

#include "TVL1Flow.h"

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 3 ) {
		std::cout << "usage: " << argv[ 0 ] << " image1 image2" << std::endl;
		return 0;
	}

	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

	Image input1( argv[ 1 ] );
	Image input2( argv[ 2 ] );
	Image flow( input1.width(), input1.height(), IFormat::GRAYALPHA_FLOAT, IALLOCATOR_CL );
	Image cflow;

	try {

	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	Flow::colorCode( cflow, flow );
	flow.save( "flow.png" );

	return 0;
}
