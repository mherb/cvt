#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>

#include <cvt/util/Time.h>

#include <cvt/vision/Flow.h>
#include <cvt/io/FloFile.h>

#include <cvt/util/Time.h>

#include "pmstereo.h"
#include "gradxy.h"

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
	try {

		Image clinput1( input1, IALLOCATOR_CL );
		Image clinput2( input2, IALLOCATOR_CL );
		Image cloutput( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clinput1g( input1.width(), input1.height(), IFormat::GRAYALPHA_FLOAT, IALLOCATOR_CL );
		Image clinput2g( input2.width(), input2.height(), IFormat::GRAYALPHA_FLOAT, IALLOCATOR_CL );


		Image clmatches1( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clmatches2( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image* clmatches[ 2 ] = { &clmatches1, &clmatches2 };


		CLKernel clpminit( _pmstereo_source, "pmstereo_init" );
		CLKernel clpmpropagate( _pmstereo_source, "pmstereo_propagate" );
		CLKernel clpmdepthmap( _pmstereo_source, "pmstereo_depthmap" );
		CLKernel clgradxy( _gradxy_source, "gradxy" );

		clgradxy.setArg( 0, clinput1g );
		clgradxy.setArg( 1, clinput1 );
		clgradxy.run( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );

		clgradxy.setArg( 0, clinput2g );
		clgradxy.setArg( 1, clinput2 );
		clgradxy.run( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );

		Time timer;
		int patchsize = 20;

		clpminit.setArg( 0, clmatches1 );
		clpminit.setArg( 1, clinput1 );
		clpminit.setArg( 2, clinput2 );
		clpminit.setArg( 3, clinput1g );
		clpminit.setArg( 4, clinput2g );
		clpminit.setArg( 5, patchsize );
		clpminit.run( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );

		for( int iter = 0; iter < 25; iter++ ) {
			int swap = iter & 1;

#if 1
			clpmdepthmap.setArg( 0, cloutput );
			clpmdepthmap.setArg( 1, *clmatches[ swap ]  );
			clpmdepthmap.runWait( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );
			cloutput.save("stereo.png");
			getchar();
#endif

			clpmpropagate.setArg( 0, *clmatches[ 1 - swap ] );
			clpmpropagate.setArg( 1, *clmatches[ swap ] );
			clpmpropagate.setArg( 2, clinput1 );
			clpmpropagate.setArg( 3, clinput2 );
			clpmpropagate.setArg( 4, clinput1g );
			clpmpropagate.setArg( 5, clinput2g );
			clpmpropagate.setArg( 6, patchsize );
			clpmpropagate.run( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );
		}


		clpmdepthmap.setArg( 0, cloutput );
		clpmdepthmap.setArg( 1, clmatches1 );
		clpmdepthmap.runWait( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );

		std::cout << timer.elapsedMilliSeconds() << " ms" << std::endl;

		cloutput.save( "stereo.png" );
		cloutput.save( "stereo.cvtraw" );

	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
