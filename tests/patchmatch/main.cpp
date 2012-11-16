#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>

#include <cvt/util/Time.h>

#include <cvt/vision/Flow.h>
#include <cvt/io/FloFile.h>

#include <cvt/util/Time.h>

#include "patchmatch.h"
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
		Image cloutput( input1.width(), input1.height(), input1.format(), IALLOCATOR_CL );
		Image clflow( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clinput1g( input1.width(), input1.height(), IFormat::GRAYALPHA_FLOAT, IALLOCATOR_CL );
		Image clinput2g( input2.width(), input2.height(), IFormat::GRAYALPHA_FLOAT, IALLOCATOR_CL );


		Image clmatches1( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clmatches2( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image* clmatches[ 2 ] = { &clmatches1, &clmatches2 };


		CLKernel clpminit( _patchmatch_source, "patchmatchInit" );
		CLKernel clpmpropagate( _patchmatch_source, "patchmatchPropagate" );
		CLKernel clpmapply( _patchmatch_source, "patchmatchApply" );
		CLKernel clpmflow( _patchmatch_source, "patchmatchToFlow" );
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

		for( int iter = 0; iter < 15; iter++ ) {
			int swap = iter & 1;

#if 1
			clpmflow.setArg( 0, clflow );
			clpmflow.setArg( 1, *clmatches[ swap ]  );
			clpmflow.runWait( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );
			clflow.save("stereo.png");

#if 0
		Image ccode( clflow.width(), clflow.height(), IFormat::BGRA_FLOAT );
		Flow::colorCode( ccode, clflow, 1.0f );
		ccode.save( "flow.png" );
#endif


			getchar();


#endif

			clpmpropagate.setArg( 0, *clmatches[ 1 - swap ] );
			clpmpropagate.setArg( 1, *clmatches[ swap ] );
			clpmpropagate.setArg( 2, clinput1 );
			clpmpropagate.setArg( 3, clinput2 );
			clpmpropagate.setArg( 4, clinput1g );
			clpmpropagate.setArg( 5, clinput2g );
			clpmpropagate.setArg( 6, iter );
			clpmpropagate.setArg( 7, patchsize );
//			clpmpropagate.setArg( 8, CLLocalSpace( ( 16 + 2 * patchsize ) * ( 16 + 2 * patchsize ) * sizeof( float ) * 4 ) );
//			clpmpropagate.setArg( 9, CLLocalSpace( ( 16 + 2 * patchsize ) * ( 16 + 2 * patchsize ) * sizeof( float ) * 2 ) );
			clpmpropagate.run( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );

		}

/*		clpmapply.setArg( 0, cloutput );
		clpmapply.setArg( 1, clmatches );
		clpmapply.setArg( 2, clinput2 );
		clpmapply.runWait( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );

		clmatches.save("pmfinished.png");*/
		

		clpmflow.setArg( 0, clflow );
		clpmflow.setArg( 1, clmatches1 );
		clpmflow.runWait( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );


		std::cout << timer.elapsedMilliSeconds() << " ms" << std::endl;

		clflow.save( "stereo.png" );
		clflow.save( "stereo.cvtraw" );

/*		Image ccode( clflow.width(), clflow.height(), IFormat::BGRA_FLOAT );
		Flow::colorCode( ccode, clflow, 1.0f );
		ccode.save( "flow.png" );*/


	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
