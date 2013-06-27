#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>
#include <cvt/math/Matrix.h>

#include <cvt/util/Time.h>

#include <cvt/vision/Flow.h>
#include <cvt/io/FloFile.h>

#include <cvt/util/Time.h>
#include <cvt/gfx/IMapScoped.h>

#include <cvt/io/RGBDParser.h>

#include "pmsfm.h"
#include "gradxy.h"

#include <cvt/cl/kernel/fill.h>
#include <cvt/cl/kernel/FlowColorCode.h>

//#include "PDROF.h"
//#include "pmstereo.h"
//#include "gradxy.h"
//#include "weight.h"

using namespace cvt;

#define KX 16
#define KY 16
#define VIEWSAMPLES 4


struct Mat3 {
	cl_float3 m[3];
};

void matrix3f_to_Mat3( Mat3& dst, const Matrix3f& m )
{
	dst.m[ 0 ].x = m[ 0 ][ 0 ];
	dst.m[ 0 ].y = m[ 0 ][ 1 ];
	dst.m[ 0 ].z = m[ 0 ][ 2 ];

	dst.m[ 1 ].x = m[ 1 ][ 0 ];
	dst.m[ 1 ].y = m[ 1 ][ 1 ];
	dst.m[ 1 ].z = m[ 1 ][ 2 ];

	dst.m[ 2 ].x = m[ 2 ][ 0 ];
	dst.m[ 2 ].y = m[ 2 ][ 1 ];
	dst.m[ 2 ].z = m[ 2 ][ 2 ];
}


int main( int argc, char** argv )
{
	if( argc != 4 ) {
		std::cout << "usage: " << argv[ 0 ] << " dataset idx1 idx2" << std::endl;
		return 0;
	}

	try {
		int patchsize = 5;
		std::vector<CLPlatform> platforms;
		std::vector<CLDevice> devs;

		RGBDParser rgbddata( argv[ 1 ] );
		int idx1, idx2;
		idx1 = String( argv[ 2 ] ).toInteger();
		idx2 = String( argv[ 3 ] ).toInteger();

/*		Matrix3f K(
				   525.0f, 0.0f, 319.5,
				   0.0f, 525.0, 239.5,
				   0.0f, 0.0f,  1.0f
				  );*/

		Matrix3f K(
535.4f, 0.0f, 320.1f,
  0.0f, 539.2f,	247.6,
  0.0f, 0.0f, 1.0f
	  );
		Matrix3f Kinv = K.inverse();

		std::cout << Kinv << std::endl;

		CLPlatform::get( platforms );
		platforms[ 0 ].devices( devs, CL_DEVICE_TYPE_GPU );
		CL::setDefaultDevice( devs[ 0 ] );

		rgbddata.setIdx( idx1 );
		rgbddata.loadNext();
		Image clinput1( rgbddata.data().rgb, IALLOCATOR_CL );
		rgbddata.setIdx( idx2 );
		rgbddata.loadNext();
		Image clinput2( rgbddata.data().rgb, IALLOCATOR_CL );
		Image clinput1g( clinput1.width(), clinput1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image clinput2g( clinput2.width(), clinput2.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image cldmap( clinput1.width(), clinput1.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );
		Image cltmp( clinput1.width(), clinput1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image cltmp2( clinput1.width(), clinput1.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );

		clinput1.save("input1.png");
		clinput2.save("input2.png");

		CLKernel clpminit( _pmsfm_source, "pmsfm_init" );
		CLKernel clpmpropagate( _pmsfm_source, "pmsfm_propagate" );
		CLKernel clpmdepthmap( _pmsfm_source, "pmsfm_depthmap" );
		CLKernel clpmnormalmap( _pmsfm_source, "pmsfm_normalmap" );
		CLKernel clpmrotmap( _pmsfm_source, "pmsfm_rotmap" );
		CLKernel clpmtransmap( _pmsfm_source, "pmsfm_transmap" );
		CLKernel clpmsmooth( _pmsfm_source, "pmsfm_smooth" );
		CLKernel clpmfwdwarp( _pmsfm_source, "pmsfm_fwdwarp" );
		CLKernel clgradxy( _gradxy_source, "gradxy" );
		CLKernel clfill( _fill_source, "fill" );
		CLKernel clcolorcode( _FlowColorCode_source, "FlowColorCode");

		CLBuffer state1( sizeof( cl_float8 ) * clinput1.width() * clinput1.height() );
		CLBuffer state2( sizeof( cl_float8 ) * clinput1.width() * clinput1.height() );

		clgradxy.setArg( 0, clinput1g );
		clgradxy.setArg( 1, clinput1 );
		clgradxy.run( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );

		clgradxy.setArg( 0, clinput2g );
		clgradxy.setArg( 1, clinput2 );
		clgradxy.run( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );

		clpminit.setArg( 0, state1 );
		clpminit.setArg<int>( 1, clinput1.width() );
		clpminit.setArg<int>( 2, clinput1.height() );
		clpminit.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );

		Mat3 clK, clKinv;
		matrix3f_to_Mat3( clK, K );
		matrix3f_to_Mat3( clKinv, Kinv );

		for( int iter = 0; iter < 200; iter++ ) {
			clpmpropagate.setArg( 0, state2 );
			clpmpropagate.setArg( 1, state1 );
			clpmpropagate.setArg( 2, clinput1 );
			clpmpropagate.setArg( 3, clinput2 );
			clpmpropagate.setArg( 4, clinput1g );
			clpmpropagate.setArg( 5, clinput2g );
			clpmpropagate.setArg( 6, sizeof( Mat3 ), &clK  );
			clpmpropagate.setArg( 7, sizeof( Mat3 ), &clKinv );
			clpmpropagate.setArg( 8, patchsize );
			clpmpropagate.setArg( 9, iter * 2 );
			clpmpropagate.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
#define DEBUG 1
#if DEBUG
			clpmdepthmap.setArg( 0, cldmap );
			clpmdepthmap.setArg( 1, state2 );
			clpmdepthmap.setArg( 2, sizeof( Mat3 ), &clKinv );
			clpmdepthmap.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cldmap.save("dmap.png");
			cldmap.save("dmap.cvtraw");

			clpmnormalmap.setArg( 0, cltmp );
			clpmnormalmap.setArg( 1, state2 );
			clpmnormalmap.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cltmp.save("nmap.png");

			clpmrotmap.setArg( 0, cltmp );
			clpmrotmap.setArg( 1, state2 );
			clpmrotmap.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cltmp.save("rotmap.png");

			clpmtransmap.setArg( 0, cltmp );
			clpmtransmap.setArg( 1, state2 );
			clpmtransmap.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cltmp.save("transmap.png");


			getchar();
#endif

			clpmpropagate.setArg( 0, state1 );
			clpmpropagate.setArg( 1, state2 );
			clpmpropagate.setArg( 2, clinput1 );
			clpmpropagate.setArg( 3, clinput2 );
			clpmpropagate.setArg( 4, clinput1g );
			clpmpropagate.setArg( 5, clinput2g );
			clpmpropagate.setArg( 6, sizeof( Mat3 ), &clK  );
			clpmpropagate.setArg( 7, sizeof( Mat3 ), &clKinv );
			clpmpropagate.setArg( 8, patchsize );
			clpmpropagate.setArg( 9, iter * 2 + 1 );
			clpmpropagate.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );


#if DEBUG

			clpmdepthmap.setArg( 0, cldmap );
			clpmdepthmap.setArg( 1, state1 );
			clpmdepthmap.setArg( 2, sizeof( Mat3 ), &clKinv );
			clpmdepthmap.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cldmap.save("dmap.png");

			clpmnormalmap.setArg( 0, cltmp );
			clpmnormalmap.setArg( 1, state1 );
			clpmnormalmap.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cltmp.save("nmap.png");

			clpmrotmap.setArg( 0, cltmp );
			clpmrotmap.setArg( 1, state1 );
			clpmrotmap.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cltmp.save("rotmap.png");

			clpmtransmap.setArg( 0, cltmp );
			clpmtransmap.setArg( 1, state1 );
			clpmtransmap.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cltmp.save("transmap.png");


			cl_float4 black = {{0.0f, 0.0f, 0.0f, 1.0f }};
			clfill.setArg( 0, cltmp );
			clfill.setArg( 1, black );
			clfill.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );

			clpmfwdwarp.setArg( 0, cltmp );
			clpmfwdwarp.setArg( 1, clinput1 );
			clpmfwdwarp.setArg( 2, state1 );
			clpmfwdwarp.setArg( 3, sizeof( Mat3 ), &clK  );
			clpmfwdwarp.setArg( 4, sizeof( Mat3 ), &clKinv );
			clpmfwdwarp.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cltmp.save("warp.png");

/*			clcolorcode.setArg( 0, cltmp2 );
			clcolorcode.setArg( 1, cltmp );
			clcolorcode.setArg( 2, 25.0f );
			clcolorcode.runWait( CLNDRange( Math::pad( clinput1.width(), KX ), Math::pad( clinput1.height(), KY ) ), CLNDRange( KX, KY ) );
			cltmp2.save("warp.png");*/

			getchar();
#endif

		}


	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
