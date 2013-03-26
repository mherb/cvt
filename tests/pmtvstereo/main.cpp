#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>

#include <cvt/util/Time.h>

#include <cvt/vision/Flow.h>
#include <cvt/io/FloFile.h>

#include <cvt/util/Time.h>

#include <cvt/gfx/ifilter/ROFFGPFilter.h>

#include "pmstereo.h"
#include "gradxy.h"

using namespace cvt;

#define KX 16
#define KY 16


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
		ROFFGPFilter* rof = new ROFFGPFilter();

		Image clinput1( input1, IALLOCATOR_CL );
		Image clinput2( input2, IALLOCATOR_CL );
		Image cloutput1( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image cloutput2( input2.width(), input2.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clinput1g( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clinput2g( input2.width(), input2.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clsmooth1( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clsmooth2( input2.width(), input2.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clsmoothtmp( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );


		Image clmatches1_1( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clmatches1_2( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image* clmatches1[ 2 ] = { &clmatches1_1, &clmatches1_2 };

		Image clmatches2_1( input2.width(), input2.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clmatches2_2( input2.width(), input2.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image* clmatches2[ 2 ] = { &clmatches2_1, &clmatches2_2 };

#define VIEWSAMPLES 3;
		struct VIEWPROP_t {
			cl_int n;
			cl_float4 samples[ 3 ];
		};

		CLBuffer viewbuf1( sizeof( VIEWPROP_t ) * input1.width() * input1.height() );
		CLBuffer viewbuf2( sizeof( VIEWPROP_t ) * input2.width() * input2.height() );


		CLKernel clpminit( _pmstereo_source, "pmstereo_init" );
		CLKernel clpmpropagate( _pmstereo_source, "pmstereo_propagate_view" );
		CLKernel clpmdepthmap( _pmstereo_source, "pmstereo_depthmap" );
		CLKernel clpmviewbufclear( _pmstereo_source, "pmstereo_viewbuf_clear" );
		CLKernel clgradxy( _gradxy_source, "gradxy" );
		CLKernel clconsistency( _pmstereo_source, "pmstereo_consistency" );
		CLKernel clfilldepthmap( _pmstereo_source, "pmstereo_fill_depthmap" );
		CLKernel cltonormaldepth( _pmstereo_source, "pmstereo_normal_depth" );

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


		clpmviewbufclear.setArg( 0, viewbuf2 );
		clpmviewbufclear.setArg( 1, ( int ) input2.width() );
		clpmviewbufclear.setArg( 2, ( int ) input2.height() );
		clpmviewbufclear.run( CLNDRange( Math::pad( clinput2.width(), 16 ), Math::pad( clinput2.height(), 16 ) ), CLNDRange( 16, 16 ) );


		float theta = 0.0f;
		for( int iter = 0; iter < 24; iter++ ) {
			int swap = iter & 1;

#if 1
			clpmdepthmap.setArg( 0, cloutput1 );
			clpmdepthmap.setArg( 1, *clmatches1[ swap ]  );
			clpmdepthmap.runWait( CLNDRange( Math::pad( clinput1.width(), 16 ), Math::pad( clinput1.height(), 16 ) ), CLNDRange( 16, 16 ) );
			cloutput1.save("stereo1.png");
			std::cout << "Wrote stereo1.png" << std::endl;

			clpmdepthmap.setArg( 0, cloutput1 );
			clpmdepthmap.setArg( 1, *clmatches2[ swap ]  );
			clpmdepthmap.runWait( CLNDRange( Math::pad( clinput2.width(), 16 ), Math::pad( clinput2.height(), 16 ) ), CLNDRange( 16, 16 ) );
			cloutput1.save("stereo2.png");
			std::cout << "Wrote stereo2.png" << std::endl;

			clconsistency.setArg( 0, cloutput1 );
			clconsistency.setArg( 1, *clmatches1[ swap ] );
			clconsistency.setArg( 2, *clmatches2[ swap ] );
			clconsistency.runWait( CLNDRange( Math::pad( clinput2.width(), 16 ), Math::pad( clinput2.height(), 16 ) ), CLNDRange( 16, 16 ) );
			cloutput1.save("stereoconsistency.png");

			clfilldepthmap.setArg( 0, cloutput2 );
			clfilldepthmap.setArg( 1, cloutput1 );
			clfilldepthmap.setArg( 2, 8.0f / 255.0f );
			clfilldepthmap.runWait( CLNDRange( Math::pad( clinput2.width(), 16 ), Math::pad( clinput2.height(), 16 ) ), CLNDRange( 16, 16 ) );

			cloutput2.save("stereofill.png");
			cloutput2.save("stereofill.cvtraw");

			getchar();
#endif

			clpmviewbufclear.setArg( 0, viewbuf1 );
			clpmviewbufclear.setArg( 1, ( int ) input1.width() );
			clpmviewbufclear.setArg( 2, ( int ) input1.height() );
			clpmviewbufclear.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );

			clpmpropagate.setArg( 0, *clmatches1[ 1 - swap ] );
			clpmpropagate.setArg( 1, *clmatches1[ swap ] );
			clpmpropagate.setArg( 2, clinput1 );
			clpmpropagate.setArg( 3, clinput2 );
			clpmpropagate.setArg( 4, clinput1g );
			clpmpropagate.setArg( 5, clinput2g );
			clpmpropagate.setArg( 6, clsmooth1 );
			clpmpropagate.setArg( 7, theta );
			clpmpropagate.setArg( 8, patchsize );
			clpmpropagate.setArg( 9, lr );
			clpmpropagate.setArg( 10, iter );
			clpmpropagate.setArg( 11, viewbuf2 );
			clpmpropagate.setArg( 12, viewbuf1 );
			clpmpropagate.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );


			clpmviewbufclear.setArg( 0, viewbuf2 );
			clpmviewbufclear.setArg( 1, ( int ) input2.width() );
			clpmviewbufclear.setArg( 2, ( int ) input2.height() );
			clpmviewbufclear.runWait( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );

			clpmpropagate.setArg( 0, *clmatches2[ 1 - swap ] );
			clpmpropagate.setArg( 1, *clmatches2[ swap ] );
			clpmpropagate.setArg( 2, clinput2 );
			clpmpropagate.setArg( 3, clinput1 );
			clpmpropagate.setArg( 4, clinput2g );
			clpmpropagate.setArg( 5, clinput1g );
			clpmpropagate.setArg( 6, clsmooth2 );
			clpmpropagate.setArg( 7, theta );
			clpmpropagate.setArg( 8, patchsize );
			clpmpropagate.setArg( 9, rl );
			clpmpropagate.setArg( 10, iter );
			clpmpropagate.setArg( 11, viewbuf1 );
			clpmpropagate.setArg( 12, viewbuf2 );
			clpmpropagate.runWait( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );

#if 0
			cltonormaldepth.setArg( 0, clsmoothtmp );
			cltonormaldepth.setArg( 1, *clmatches1[ 1 - swap ] );
			cltonormaldepth.setArg( 2, lr );
			cltonormaldepth.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );

			clsmoothtmp.save("stereosmoothorig.png");
			rof->apply( clsmooth1, clsmoothtmp, 0.05f, 10 );
			clsmooth1.save("stereosmooth.png");

			cltonormaldepth.setArg( 0, clsmoothtmp );
			cltonormaldepth.setArg( 1, *clmatches2[ 1 - swap ] );
			cltonormaldepth.setArg( 2, rl );
			cltonormaldepth.runWait( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );

			rof->apply( clsmooth2, clsmoothtmp );

			theta = Math::smoothstep( ( iter / 23.0f )  ) * 0.1f;
#endif

		}

		std::cout << timer.elapsedMilliSeconds() << " ms" << std::endl;

		clconsistency.setArg( 0, cloutput1 );
		clconsistency.setArg( 1, *clmatches1[ 1 ] );
		clconsistency.setArg( 2, *clmatches2[ 1 ] );
		clconsistency.runWait( CLNDRange( Math::pad( clinput2.width(), 16 ), Math::pad( clinput2.height(), 16 ) ), CLNDRange( 16, 16 ) );
		cloutput1.save("stereoconsistency.png");

		Image cloutputfinal( input1.width(), input1.height(), IFormat::GRAY_UINT8, IALLOCATOR_CL );
		clfilldepthmap.setArg( 0, cloutputfinal );
		clfilldepthmap.setArg( 1, cloutput1 );
		clfilldepthmap.setArg( 2, ( 8.0f / 255.0f ) );
		clfilldepthmap.runWait( CLNDRange( Math::pad( clinput2.width(), 16 ), Math::pad( clinput2.height(), 16 ) ), CLNDRange( 16, 16 ) );

		cloutputfinal.save( "stereofinal.png" );

		Image cloutputfinal2( input1.width(), input1.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );
		clfilldepthmap.setArg( 0, cloutputfinal2 );
		clfilldepthmap.setArg( 1, cloutput1 );
		clfilldepthmap.setArg( 2, ( 8.0f / 255.0f ) );
		clfilldepthmap.runWait( CLNDRange( Math::pad( clinput2.width(), 16 ), Math::pad( clinput2.height(), 16 ) ), CLNDRange( 16, 16 ) );
		cloutputfinal2.save( "stereofinal.cvtraw" );

	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
