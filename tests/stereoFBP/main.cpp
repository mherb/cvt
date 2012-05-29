#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLBuffer.h>
#include <cvt/util/String.h>
#include <cvt/util/Time.h>

#include <cvt/gfx/ifilter/ColorCode.h>
#include <cvt/cl/kernel/stereo/stereoCVFBP.h>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 5 ) {
		std::cout << "usage: " << argv[ 0 ] << " left-image right-image max-disp iter" << std::endl;
		return 0;
	}

	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

	int depth = String( argv[ 3 ] ).toInteger();
	int iter = String( argv[ 4 ] ).toInteger();
	Image left( argv[ 1 ] );
	Image right( argv[ 2 ] );

	Image clleft( left, IALLOCATOR_CL );
	Image clright( right, IALLOCATOR_CL );
	Image cldmap( left.width(), left.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );

	try {
		CLKernel kerncv( _stereoCVFBP_source, "stereoCV_FBP_AD" );
		CLKernel kerncvfbp( _stereoCVFBP_source, "stereoCV_FBP" );
		CLKernel kerncvwta( _stereoCVFBP_source, "stereoCV_FBP_WTA" );


		CLBuffer cv( sizeof( cl_float2 ) * left.width() * left.height() * depth  );

		Time t;

		kerncv.setArg( 0, cv );
		kerncv.setArg( 1, depth );
		kerncv.setArg( 2, clright );
		kerncv.setArg( 3, clleft );
		kerncv.setArg( 4, CLLocalSpace( sizeof( cl_float ) * 1 * ( depth + 256 ) ) );
		kerncv.run( CLNDRange( Math::pad( left.width(), 256 ), left.height() ), CLNDRange( 256, 1 ) );

		for( int i = 0; i < iter; i++ ) {
			kerncvfbp.setArg( 0, cv );
			kerncvfbp.setArg<int>( 1, left.width() );
			kerncvfbp.setArg<int>( 2, left.height() );
			kerncvfbp.setArg( 3, depth );
			kerncvfbp.setArg( 4, i );
			kerncvfbp.run( CLNDRange( Math::pad( left.width() / 2, 16 ), Math::pad( left.height(), 16 ) ), CLNDRange( 16, 16 ) );
		}

		kerncvwta.setArg( 0, cldmap );
		kerncvwta.setArg( 1, cv );
		kerncvwta.setArg( 2, depth );
		kerncvwta.runWait( CLNDRange( Math::pad16( left.width() ), Math::pad16( left.height() ) ), CLNDRange( 16, 16 ) );

		std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;
		Image tmp;
		ColorCode::apply( tmp, cldmap, 1.0f, 0.0f );
		tmp.save("dmap-fbp-color.png");
		cldmap.save( "dmap-fbp.png" );

	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
