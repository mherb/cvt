#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>

#include <cvt/util/Time.h>

#include <cvt/vision/Flow.h>
#include <cvt/io/FloFile.h>
#include <cvt/vision/Vision.h>

#include <cvt/util/Time.h>
#include <cvt/vision/CameraCalibration.h>

#include "patchmatch.h"
#include "gradxy.h"

using namespace cvt;


int main( int argc, char** argv )
{
	if( argc != 7 ) {
		std::cout << "usage: " << argv[ 0 ] << " cam0 cam1 cam2 image0 image1 image2" << std::endl;
		return 0;
	}

	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

	Matrix4f mat[ 6 ];

	CameraCalibration cam0, cam1, cam2;
	cam0.load( argv[ 1 ] );
	cam1.load( argv[ 2 ] );
	cam2.load( argv[ 3 ] );

	Matrix3f htmp;
	Vision::planeSweepHomography( htmp, cam0.intrinsics(), cam0.extrinsics(), cam1.intrinsics(), cam1.extrinsics(), Vector3f( 0.0f, 0.0f, 1.0f ), 10.0f );
	mat[ 0 ] = cam0.intrinsics().pseudoInverse().toMatrix4();
	mat[ 1 ] = cam0.extrinsics();
	mat[ 2 ] = cam1.intrinsics().toMatrix4();
	mat[ 3 ] = cam1.extrinsics();
	mat[ 4 ] = cam2.intrinsics().toMatrix4();
	mat[ 5 ] = cam2.extrinsics();


	Image input1( argv[ 4 ] );
	Image input2( argv[ 5 ] );
	Image input3( argv[ 6 ] );

	try {

		Image clinput1( input1, IALLOCATOR_CL );
		Image clinput2( input2, IALLOCATOR_CL );
		Image clinput3( input3, IALLOCATOR_CL );
		Image cloutput( input1.width(), input1.height(), input1.format(), IALLOCATOR_CL );
		Image clflow( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clinput1g( input1.width(), input1.height(), IFormat::GRAYALPHA_FLOAT, IALLOCATOR_CL );
		Image clinput2g( input2.width(), input2.height(), IFormat::GRAYALPHA_FLOAT, IALLOCATOR_CL );
		Image clinput3g( input3.width(), input3.height(), IFormat::GRAYALPHA_FLOAT, IALLOCATOR_CL );

		Image clmatches1( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clmatches2( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image* clmatches[ 2 ] = { &clmatches1, &clmatches2 };

//		CLKernel clplanesweep( _patchmatch_source, "planeSweep" );
		CLKernel clpminit( _patchmatch_source, "patchmatchInit" );
		CLKernel clpmpropagate( _patchmatch_source, "patchmatchPropagate" );
		CLKernel clpmapply( _patchmatch_source, "patchmatchApply" );
		CLKernel clpmflow( _patchmatch_source, "patchmatchToFlow" );
		CLKernel clgradxy( _gradxy_source, "gradxy" );

		CLBuffer mats( mat, sizeof( Matrix4f ) * 6, CL_MEM_READ_ONLY );

/*		clplanesweep.setArg( 0, cloutput );
		clplanesweep.setArg( 1, clinput2 );
		clplanesweep.setArg( 2, mats );
		clplanesweep.setArg( 3, 10 );
		clplanesweep.run( CLNDRange( Math::pad( cloutput.width(), 16 ), Math::pad( cloutput.height(), 16 ) ), CLNDRange( 16, 16 ) );

		cloutput.save("planesweep.png");
		return 0;*/


		clgradxy.setArg( 0, clinput1g );
		clgradxy.setArg( 1, clinput1 );
		clgradxy.run( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );

		clgradxy.setArg( 0, clinput2g );
		clgradxy.setArg( 1, clinput2 );
		clgradxy.run( CLNDRange( Math::pad( clinput2.width(), 16 ), Math::pad( clinput2.height(), 16 ) ), CLNDRange( 16, 16 ) );

		clgradxy.setArg( 0, clinput3g );
		clgradxy.setArg( 1, clinput3 );
		clgradxy.run( CLNDRange( Math::pad( clinput3.width(), 16 ), Math::pad( clinput3.height(), 16 ) ), CLNDRange( 16, 16 ) );

		Time timer;
		int patchsize = 9;

		clpminit.setArg( 0, clmatches1 );
		clpminit.setArg( 1, clinput1 );
		clpminit.setArg( 2, clinput2 );
		clpminit.setArg( 3, clinput3 );
		clpminit.setArg( 4, clinput1g );
		clpminit.setArg( 5, clinput2g );
		clpminit.setArg( 6, clinput2g );
		clpminit.setArg( 7, mats );
		clpminit.setArg( 8, patchsize );
		clpminit.run( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );

		for( int iter = 0; iter < 100; iter++ ) {
			int swap = iter & 1;

#if 1
			clpmflow.setArg( 0, clflow );
			clpmflow.setArg( 1, *clmatches[ swap ]  );
			clpmflow.setArg( 2, mats  );
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
			clpmpropagate.setArg( 4, clinput3 );
			clpmpropagate.setArg( 5, clinput1g );
			clpmpropagate.setArg( 6, clinput2g );
			clpmpropagate.setArg( 7, clinput3g );
			clpmpropagate.setArg( 8, mats );
			clpmpropagate.setArg( 9, iter );
			clpmpropagate.setArg( 10, patchsize );
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
