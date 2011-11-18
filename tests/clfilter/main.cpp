#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>

#include <cvt/util/Time.h>

#include <gfx/ifilter/IntegralFilter.h>
#include <gfx/ifilter/BoxFilter.h>
#include <gfx/ifilter/GuidedFilter.h>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 2 ) {
		std::cout << "usage: " << argv[ 0 ] << " image" << std::endl;
		return 0;
	}

	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

	Image input( argv[ 1 ] );
	Image cl( input, IALLOCATOR_CL );
	Image clout( input.width(), input.height(), input.format(), IALLOCATOR_CL );


	try {
//		IntegralFilter ifilter;
//		BoxFilter bfilter;
		GuidedFilter gfilter;

//		ifilter.apply( clintegral, cl );
//		bfilter.apply( cl, clintegral, 20 );
		Time t;
		gfilter.apply( clout, cl, cl, 16, 0.01f );
		std::cout << "GuidedFilter: " << t.elapsedMilliSeconds() << std::endl;

		clout.save( "cltest.png" );

	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
