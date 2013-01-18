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
		Image cloutput1( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image cloutput2( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clinput1g( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clinput2g( input2.width(), input2.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );


		Image clmatches1_1( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clmatches1_2( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image* clmatches1[ 2 ] = { &clmatches1_1, &clmatches1_2 };

		Image clmatches2_1( input2.width(), input2.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clmatches2_2( input2.width(), input2.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image* clmatches2[ 2 ] = { &clmatches2_1, &clmatches2_2 };


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
		int patchsize = 2;
		int lr = 1;
		int rl = 0;

		clpminit.setArg( 0, *clmatches1[ 0 ] );
		clpminit.setArg( 1, clinput1 );
		clpminit.setArg( 2, clinput2 );
		clpminit.setArg( 3, clinput1g );
		clpminit.setArg( 4, clinput2g );
		clpminit.setArg( 5, patchsize );
		clpminit.setArg( 6, lr );
		clpminit.run( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );

		clpminit.setArg( 0, *clmatches2[ 0 ] );
		clpminit.setArg( 1, clinput2 );
		clpminit.setArg( 2, clinput1 );
		clpminit.setArg( 3, clinput2g );
		clpminit.setArg( 4, clinput1g );
		clpminit.setArg( 5, patchsize );
		clpminit.setArg( 6, rl );
		clpminit.run( CLNDRange( Math::pad( clinput2.width(), 16 ), Math::pad( clinput2.height(), 16 ) ), CLNDRange( 16, 16 ) );

		for( int iter = 0; iter < 15; iter++ ) {
			int swap = iter & 1;

#if 1
			clpmdepthmap.setArg( 0, cloutput1 );
			clpmdepthmap.setArg( 1, *clmatches1[ swap ]  );
			clpmdepthmap.runWait( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );
			cloutput1.save("stereo.png");

			clpmdepthmap.setArg( 0, cloutput1 );
			clpmdepthmap.setArg( 1, *clmatches2[ swap ]  );
			clpmdepthmap.runWait( CLNDRange( Math::pad( clinput2.width(), 16 ), Math::pad( clinput2.height(), 16 ) ), CLNDRange( 16, 16 ) );
			cloutput1.save("stereo2.png");

			getchar();
#endif

			clpmpropagate.setArg( 0, *clmatches1[ 1 - swap ] );
			clpmpropagate.setArg( 1, *clmatches1[ swap ] );
			clpmpropagate.setArg( 2, clinput1 );
			clpmpropagate.setArg( 3, clinput2 );
			clpmpropagate.setArg( 4, clinput1g );
			clpmpropagate.setArg( 5, clinput2g );
			clpmpropagate.setArg( 6, patchsize );
			clpmpropagate.setArg( 7, lr );
			clpmpropagate.setArg( 8, iter );
			clpmpropagate.run( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );

			clpmpropagate.setArg( 0, *clmatches2[ 1 - swap ] );
			clpmpropagate.setArg( 1, *clmatches2[ swap ] );
			clpmpropagate.setArg( 2, clinput2 );
			clpmpropagate.setArg( 3, clinput1 );
			clpmpropagate.setArg( 4, clinput2g );
			clpmpropagate.setArg( 5, clinput1g );
			clpmpropagate.setArg( 6, patchsize );
			clpmpropagate.setArg( 7, rl );
			clpmpropagate.setArg( 8, iter );
			clpmpropagate.run( CLNDRange( Math::pad( clinput2.width(), 16 ), Math::pad( clinput2.height(), 16 ) ), CLNDRange( 16, 16 ) );
		}

		std::cout << timer.elapsedMilliSeconds() << " ms" << std::endl;

		clpmdepthmap.setArg( 0, cloutput1 );
		clpmdepthmap.setArg( 1, clmatches1_1 );
		clpmdepthmap.runWait( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );
		cloutput1.save( "stereo.png" );
		cloutput1.save( "stereo.cvtraw" );

		clpmdepthmap.setArg( 0, cloutput2 );
		clpmdepthmap.setArg( 1, clmatches2_1 );
		clpmdepthmap.runWait( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );
		cloutput2.save( "stereo2.png" );
		cloutput2.save( "stereo2.cvtraw" );




	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
