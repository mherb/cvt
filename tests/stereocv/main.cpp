#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLBuffer.h>
#include <cvt/util/String.h>
#include <cvt/util/Time.h>

#include <cvt/cl/kernel/stereo/stereoCV.h>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 4 ) {
		std::cout << "usage: " << argv[ 0 ] << " left-image right-image max-disp" << std::endl;
		return 0;
	}

	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

	int depth = String( argv[ 3 ] ).toInteger();
	Image left( argv[ 1 ] );
	Image right( argv[ 2 ] );

	Image clleft( left, IALLOCATOR_CL );
	Image clright( right, IALLOCATOR_CL );
	Image cldmap( left.width(), left.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );

	try {
		CLKernel kerncv( _stereoCV_source, "stereoCV" );
		CLKernel kerncvwta( _stereoCV_source, "stereoCV_WTA" );

		CLBuffer cv( sizeof( cl_float ) * left.width() * left.height() * depth  );

		Time t;

		kerncv.setArg( 0, cv );
		kerncv.setArg( 1, depth );
		kerncv.setArg( 2, clright );
		kerncv.setArg( 3, clleft );
		kerncv.setArg( 4, CLLocalSpace( sizeof( cl_float4 ) * ( depth + 256 ) ) );
		kerncv.run( CLNDRange( Math::pad( left.width(), 256 ), left.height() ), CLNDRange( 256, 1 ) );

		kerncvwta.setArg( 0, cldmap );
		kerncvwta.setArg( 1, cv );
		kerncvwta.setArg( 2, depth );
		kerncvwta.runWait( CLNDRange( Math::pad16( left.width() ), Math::pad16( left.height() ) ), CLNDRange( 16, 16 ) );

		std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;

		cldmap.save( "dmap.png" );

	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
