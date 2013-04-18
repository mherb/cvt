#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>

#include <cvt/util/Time.h>

#include <cvt/vision/Flow.h>
#include <cvt/io/FloFile.h>

#include <cvt/util/Time.h>
#include <cvt/gfx/IMapScoped.h>

#include <cvt/gfx/ifilter/ROFFGPFilter.h>

#include "PDROF.h"
//#include "PDROFInpaint.h"

#include "pmstereo.h"
#include "gradxy.h"
#include "weight.h"

using namespace cvt;

#define KX 16
#define KY 16
#define VIEWSAMPLES 4


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
//		ROFFGPFilter* rof = new ROFFGPFilter();
		PDROF pdrof;

		Image clinput1( input1, IALLOCATOR_CL );
		Image clinput2( input2, IALLOCATOR_CL );
		Image cloutput1( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image cloutput2( input2.width(), input2.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clinput1g( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clinput2g( input2.width(), input2.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clsmooth1( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clsmooth2( input2.width(), input2.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clsmoothtmp( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clsmoothtmp2( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image cloccimg( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clweightl( input1.width(), input1.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );
		Image clweightr( input2.width(), input2.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );


		Image clmatches1_1( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clmatches1_2( input1.width(), input1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image* clmatches1[ 2 ] = { &clmatches1_1, &clmatches1_2 };

		Image clmatches2_1( input2.width(), input2.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clmatches2_2( input2.width(), input2.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image* clmatches2[ 2 ] = { &clmatches2_1, &clmatches2_2 };

		struct VIEWPROP_t {
			cl_int n;
			cl_float4 samples[ VIEWSAMPLES ];
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
		CLKernel clfill( _pmstereo_source, "pmstereo_fill_state" );
		CLKernel cltonormaldepth( _pmstereo_source, "pmstereo_normal_depth" );
		CLKernel clpmclear( _pmstereo_source, "pmstereo_clear" );
		CLKernel cloccmap( _pmstereo_source, "pmstereo_occmap" );
		CLKernel weight(_weight_source, "weight" );

		weight.setArg( 0, clweightl );
		weight.setArg( 1, clinput1 );
		weight.run( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );

		weight.setArg( 0, clweightr );
		weight.setArg( 1, clinput2 );
		weight.run( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );

		clgradxy.setArg( 0, clinput1g );
		clgradxy.setArg( 1, clinput1 );
		clgradxy.run( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );

		clgradxy.setArg( 0, clinput2g );
		clgradxy.setArg( 1, clinput2 );
		clgradxy.run( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );

		clpmclear.setArg( 0, clsmooth1 );
		clpmclear.run( CLNDRange( Math::pad( clsmooth1.width(), KX ), Math::pad( clsmooth1.height(), KY ) ), CLNDRange( KX, KY ) );

		clpmclear.setArg( 0, clsmooth2 );
		clpmclear.run( CLNDRange( Math::pad( clsmooth2.width(), KX ), Math::pad( clsmooth2.height(), KY ) ), CLNDRange( KX, KY ) );


		Time timer;
		int patchsize = 20;
		int lr = 1;
		int rl = 0;


		clpminit.setArg( 0, *clmatches1[ 0 ] );
		clpminit.setArg( 1, clinput1 );
		clpminit.setArg( 2, clinput2 );
		clpminit.setArg( 3, clinput1g );
		clpminit.setArg( 4, clinput2g );
		clpminit.setArg( 5, patchsize );
		clpminit.setArg( 6, lr );
		clpminit.run( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );

		clpminit.setArg( 0, *clmatches2[ 0 ] );
		clpminit.setArg( 1, clinput2 );
		clpminit.setArg( 2, clinput1 );
		clpminit.setArg( 3, clinput2g );
		clpminit.setArg( 4, clinput1g );
		clpminit.setArg( 5, patchsize );
		clpminit.setArg( 6, rl );
		clpminit.run( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );


		clpmviewbufclear.setArg( 0, viewbuf2 );
		clpmviewbufclear.setArg( 1, ( int ) input2.width() );
		clpmviewbufclear.setArg( 2, ( int ) input2.height() );
		clpmviewbufclear.run( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );


		float theta = 0.0f;
		for( int iter = 0; iter < 34; iter++ ) {
			int swap = iter & 1;

#if 1
			clpmdepthmap.setArg( 0, cloutput1 );
			clpmdepthmap.setArg( 1, *clmatches1[ swap ]  );
			clpmdepthmap.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cloutput1.save("stereo1.png");
			std::cout << "Wrote stereo1.png" << std::endl;

			clpmdepthmap.setArg( 0, cloutput1 );
			clpmdepthmap.setArg( 1, *clmatches2[ swap ]  );
			clpmdepthmap.runWait( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );
			cloutput1.save("stereo2.png");
			std::cout << "Wrote stereo2.png" << std::endl;

			clconsistency.setArg( 0, cloutput1 );
			clconsistency.setArg( 1, *clmatches1[ swap ] );
			clconsistency.setArg( 2, *clmatches2[ swap ] );
			clconsistency.setArg( 3, lr );
			clconsistency.runWait( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );
			cloutput1.save("stereoconsistency.png");

			clfilldepthmap.setArg( 0, cloutput2 );
			clfilldepthmap.setArg( 1, cloutput1 );
			clfilldepthmap.setArg( 2, 4.0f / 255.0f );
			clfilldepthmap.runWait( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );

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
#if 1
			cloccmap.setArg( 0, cloccimg );
			cloccmap.setArg( 1, *clmatches1[ 1 - swap ] );
			cloccmap.setArg( 2, *clmatches2[ 1 - swap ] );
			cloccmap.setArg( 3, 0.5f );
			cloccmap.setArg( 4, lr );
			cloccmap.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cloccimg.save("stereoocc1.png");

			cloccmap.setArg( 0, cloccimg );
			cloccmap.setArg( 1, *clmatches2[ 1 - swap ] );
			cloccmap.setArg( 2, *clmatches1[ 1 - swap ] );
			cloccmap.setArg( 3, 0.5f );
			cloccmap.setArg( 4, rl );
			cloccmap.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cloccimg.save("stereoocc2.png");

#endif
#if 1

			clconsistency.setArg( 0, clsmoothtmp );
			clconsistency.setArg( 1, *clmatches1[ 1 - swap ] );
			clconsistency.setArg( 2, *clmatches2[ 1 - swap ] );
			clconsistency.setArg( 3, lr );
			clconsistency.runWait( CLNDRange( Math::pad( clsmoothtmp.width(), KX ), Math::pad( clsmoothtmp.height(), KY ) ), CLNDRange( KX, KY ) );

			clfill.setArg( 0, clsmoothtmp2 );
			clfill.setArg( 1, clsmoothtmp );
			clfill.setArg( 2, lr );
			clfill.runWait( CLNDRange( Math::pad( clsmoothtmp.width(), KX ), Math::pad( clsmoothtmp.height(), KY ) ), CLNDRange( KX, KY ) );

//			clsmoothtmp2.save("stereosmoothorig1.png");
			pdrof.apply( clsmooth1, clsmoothtmp2, clweightl, theta * 50.0f + 5.0f, 200 );
//			clsmooth1.save("stereosmooth1.png");

			clconsistency.setArg( 0, clsmoothtmp );
			clconsistency.setArg( 1, *clmatches2[ 1 - swap ] );
			clconsistency.setArg( 2, *clmatches1[ 1 - swap ] );
			clconsistency.setArg( 3, rl );
			clconsistency.runWait( CLNDRange( Math::pad( clsmoothtmp.width(), KX ), Math::pad( clsmoothtmp.height(), KY ) ), CLNDRange( KX, KY ) );

			clfill.setArg( 0, clsmoothtmp2 );
			clfill.setArg( 1, clsmoothtmp );
			clfill.setArg( 2, rl );
			clfill.runWait( CLNDRange( Math::pad( clsmoothtmp.width(), KX ), Math::pad( clsmoothtmp.height(), KY ) ), CLNDRange( KX, KY ) );

//			clsmoothtmp2.save("stereosmoothorig2.png");
			pdrof.apply( clsmooth2, clsmoothtmp2, clweightr, theta * 50.0f + 5.0f, 200 );
//			clsmooth2.save("stereosmooth2.png");


			theta = Math::smoothstep<float>( ( ( iter - 4 ) / ( 33.0f - 4.0f ) )  ) * 1.0f;
#endif

#if 0
			cloccmap.setArg( 0, cloccimg );
			cloccmap.setArg( 1, *clmatches1[ 1 - swap ] );
			cloccmap.setArg( 2, *clmatches2[ 1 - swap ] );
			cloccmap.setArg( 3, 0.5f );
			cloccmap.setArg( 4, lr );
			cloccmap.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cloccimg.save("stereoocc1.png");

			cltonormaldepth.setArg( 0, clsmoothtmp );
			cltonormaldepth.setArg( 1, *clmatches1[ 1 - swap ] );
			cltonormaldepth.setArg( 2, lr );
			cltonormaldepth.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			clsmoothtmp.save("stereosmoothorig1.png");

			pdrof.apply( clsmooth1, clsmoothtmp, clweightl, cloccimg, theta * 50.0f + 5.0f, 300 );
			clsmooth1.save("stereosmooth1.png");

			cloccmap.setArg( 0, cloccimg );
			cloccmap.setArg( 1, *clmatches2[ 1 - swap ] );
			cloccmap.setArg( 2, *clmatches1[ 1 - swap ] );
			cloccmap.setArg( 3, 0.5f );
			cloccmap.setArg( 4, rl );
			cloccmap.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cloccimg.save("stereoocc2.png");

			cltonormaldepth.setArg( 0, clsmoothtmp );
			cltonormaldepth.setArg( 1, *clmatches2[ 1 - swap ] );
			cltonormaldepth.setArg( 2, rl );
			cltonormaldepth.runWait( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );

			clsmoothtmp.save("stereosmoothorig2.png");
			pdrof.apply( clsmooth2, clsmoothtmp, clweightr, cloccimg, theta * 50.0f + 5.0f, 300 );
			clsmooth2.save("stereosmooth2.png");

			theta = Math::smoothstep<float>( ( ( iter - 4 ) / ( 33.0f - 4.0f ) )  ) * 1.0f;
#endif


#if 0
			cltonormaldepth.setArg( 0, clsmoothtmp );
			cltonormaldepth.setArg( 1, *clmatches1[ 1 - swap ] );
			cltonormaldepth.setArg( 2, lr );
			cltonormaldepth.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );

			clsmoothtmp.save("stereosmoothorig1.png");
			rof->apply( clsmooth1, clsmoothtmp, 1.0f / ( theta * 50.0f + 5.0f ), 40 );
			clsmooth1.save("stereosmooth1.png");

			cltonormaldepth.setArg( 0, clsmoothtmp );
			cltonormaldepth.setArg( 1, *clmatches2[ 1 - swap ] );
			cltonormaldepth.setArg( 2, rl );
			cltonormaldepth.runWait( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );

			clsmoothtmp.save("stereosmoothorig2.png");
			rof->apply( clsmooth2, clsmoothtmp, 1.0f / ( theta * 50.0f + 5.0f ), 40 );
			clsmooth2.save("stereosmooth2.png");

			//theta = theta * 2.0f + 0.001f;
//			theta = Math::smoothstep<float>( ( ( iter - 10.0f ) / ( 47.0f - 10.0f ) )  ) * 1.0f;
//			theta = Math::smoothstep<float>( ( ( iter - 10 ) / ( 47.0f - 10.0f ) )  ) * 10.0f;

			theta = Math::smoothstep<float>( ( ( iter - 5 ) / ( 23.0f - 5.0f ) )  ) * 1.0f;
#endif

		}

		std::cout << timer.elapsedMilliSeconds() << " ms" << std::endl;

		clconsistency.setArg( 0, cloutput1 );
		clconsistency.setArg( 1, *clmatches1[ 1 ] );
		clconsistency.setArg( 2, *clmatches2[ 1 ] );
		clconsistency.setArg( 3, lr );
		clconsistency.runWait( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );
		cloutput1.save("stereoconsistency.png");

		Image cloutputfinal( input1.width(), input1.height(), IFormat::GRAY_UINT8, IALLOCATOR_CL );
		clfilldepthmap.setArg( 0, cloutputfinal );
		clfilldepthmap.setArg( 1, cloutput1 );
		clfilldepthmap.setArg( 2, ( 4.0f / 255.0f ) );
		clfilldepthmap.runWait( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );
		cloutputfinal.save( "stereofinal.png" );

		Image cloutputfinal2( input1.width(), input1.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );
		clfilldepthmap.setArg( 0, cloutputfinal2 );
		clfilldepthmap.setArg( 1, cloutput1 );
		clfilldepthmap.setArg( 2, ( 4.0f / 255.0f ) );
		clfilldepthmap.runWait( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );
		cloutputfinal2.save( "stereofinal.cvtraw" );

		clconsistency.setArg( 0, cloutput2 );
		clconsistency.setArg( 1, *clmatches2[ 1 ] );
		clconsistency.setArg( 2, *clmatches1[ 1 ] );
		clconsistency.setArg( 3, rl );
		clconsistency.runWait( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );


		clfilldepthmap.setArg( 0, cloutputfinal2 );
		clfilldepthmap.setArg( 1, cloutput2 );
		clfilldepthmap.setArg( 2, ( 4.0f / 255.0f ) );
		clfilldepthmap.runWait( CLNDRange( Math::pad( clinput2.width(), KX ), Math::pad( clinput2.height(), KY ) ), CLNDRange( KX, KY ) );
		cloutputfinal2.save( "stereofinal2.cvtraw" );


	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
