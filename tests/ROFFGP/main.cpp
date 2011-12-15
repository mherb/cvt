
#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>

#include <cvt/gfx/ifilter/ROFFGPFilter.h>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 2 ) {
		std::cout << "usage: " << argv[ 0 ] << " image" << std::endl;
		return 0;
	}

	try {
	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

	Image input( argv[ 1 ] );
	Image climg( input.width(), input.height(), input.format(), IALLOCATOR_CL );
//	climg = input;
//	input.convert( climg );

	Image output( input.width(), input.height(), input.format(), IALLOCATOR_CL );

//		climg.save("roffgporig.png");
		ROFFGPFilter filter;
		filter.apply( output, climg, 0.1f, 20 );
//		output.save( "roffgp.png" );
	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}


	return 0;
}
