#include "ROFFGPFilter.h"

#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>

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
	Image climg( input.width(), input.height(), input.format(), IALLOCATOR_CL );
	input.convert( climg );

	Image output( input.width(), input.height(), input.format(), IALLOCATOR_CL );

	ROFFGPFilter filter;
	filter.apply( output, climg );

	output.save( "roffgp.png" );

	return 0;
}
