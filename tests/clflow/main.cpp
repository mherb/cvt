#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>

#include <cvt/util/Time.h>

#include <cvt/vision/Flow.h>
#include <cvt/io/FloFile.h>

#include "TVL1Flow.h"

using namespace cvt;


int main( int argc, char** argv )
{
	if( argc != 3 && argc != 4 ) {
		std::cout << "usage: " << argv[ 0 ] << " image1 image2" << std::endl;
		return 0;
	}

	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

	Image input1( argv[ 1 ] );
	Image input2( argv[ 2 ] );

	Image output;
	Image ccode;

	try {
		TVL1Flow flow( 0.8, 8 );

		Time t;
		flow.apply( output, input1, input2 );

		std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;

		ccode.reallocate( output.width(), output.height(), IFormat::BGRA_FLOAT );
		Flow::colorCode( ccode, output, 2.0f );
		ccode.save( "flow.png" );

		if( argc == 4 ) {
			Image gt;
			FloFile::FloReadFile( gt, argv[ 3 ] );
			float aee = Flow::AEE( output, gt );
			float aae = Flow::AAE( output, gt );
			std::cout << "AEE: " << aee << std::endl;
			std::cout << "AAE: " << aae << std::endl;
		}

	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
