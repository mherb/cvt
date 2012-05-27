#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLBuffer.h>
#include <cvt/util/String.h>
#include <cvt/util/Time.h>

#include <cvt/gfx/ifilter/ColorCode.h>
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
		CLKernel kerncv( _stereoCV_source, "stereoCV_GRAY_AD" );
		CLKernel kerncvbox( _stereoCV_source, "stereoCV_boxfilter" );
		CLKernel kerncvwta( _stereoCV_source, "stereoCV_WTA" );


		CLBuffer cv( sizeof( cl_float ) * left.width() * left.height() * depth  );
		CLBuffer cv2( sizeof( cl_float ) * left.width() * left.height() * depth  );

		Time t;

		kerncv.setArg( 0, cv );
		kerncv.setArg( 1, depth );
		kerncv.setArg( 2, clright );
		kerncv.setArg( 3, clleft );
		kerncv.setArg( 4, CLLocalSpace( sizeof( cl_float ) * 1 * ( depth + 256 ) ) );
		kerncv.run( CLNDRange( Math::pad( left.width(), 256 ), left.height() ), CLNDRange( 256, 1 ) );

#if 1
#define CVBOXWG 16
		int r = 3;
		kerncvbox.setArg( 0, cv2 );
		kerncvbox.setArg( 1, cv );
		kerncvbox.setArg<int>( 2, left.width() );
		kerncvbox.setArg<int>( 3, left.height() );
		kerncvbox.setArg( 4, depth );
		kerncvbox.setArg( 5, r );
		kerncvbox.setArg( 6, CLLocalSpace( sizeof( cl_float ) * ( CVBOXWG + 2 * r ) * ( CVBOXWG + 2 * r ) ) );
		kerncvbox.run( CLNDRange( Math::pad( left.width(), CVBOXWG ), Math::pad( left.height(), CVBOXWG ), depth ), CLNDRange( CVBOXWG, CVBOXWG, 1 ) );
#endif

		kerncvwta.setArg( 0, cldmap );
		kerncvwta.setArg( 1, cv2 );
		kerncvwta.setArg( 2, depth );
		kerncvwta.runWait( CLNDRange( Math::pad16( left.width() ), Math::pad16( left.height() ) ), CLNDRange( 16, 16 ) );

		std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;
		Image tmp;
		ColorCode::apply( tmp, cldmap, 1.0f, 0.0f );
		tmp.save("dmap-color.png");

		cldmap.save( "dmap.png" );

	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
