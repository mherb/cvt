#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>

#include <cvt/util/Time.h>

#include <cvt/gfx/ifilter/TVL1Stereo.h>

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
		TVL1Stereo stereo( 0.80, 15 );

		Time t;
		stereo.apply( output, input1, input2 );

		std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;

		output.add( -10.0f );
		output.mul( -1.0f / 100.0f );
		output.save("stereo.png");


	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
