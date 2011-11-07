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

	Image output( input.width(), input.height(), IFormat::floatEquivalent( input.format() ), IALLOCATOR_CL );

	try {
		climg.save("roffgporig.png");
		ROFFGPFilter filter;
		filter.apply( output, climg, 0.05f, 10 );
		output.save( "roffgp.png" );
	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}


	return 0;
}
