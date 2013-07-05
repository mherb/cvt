#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>

#include <cvt/util/Time.h>

#include <cvt/vision/PMHuberStereo.h>


using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 3 && argc != 4 ) {
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
		Image clinput1( input1, IALLOCATOR_CL );
		Image clinput2( input2, IALLOCATOR_CL );
		Image dmap( input1, IALLOCATOR_CL );

		PMHuberStereo pmhs;

		pmhs.depthMap( dmap, clinput1, clinput2, 25, 20, 30, 4 );

		dmap.save("pmh.png");

	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
