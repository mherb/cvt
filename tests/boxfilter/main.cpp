#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/gfx/ifilter/BoxFilter.h>
#include <cvt/util/Time.h>

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

	Image tmp( argv[ 1 ] );
	Image in( tmp, IALLOCATOR_CL );
	Image out;

	BoxFilter box;
	Time t;
	box.apply( out, in, 1, false );
	std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;

	out.save( "box.png" );
}
