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

	Image clleft( left.width(), left.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );
	Image clright( left.width(), left.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );
	Image cldmap( left.width(), left.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );
	left.convert( clleft );
	right.convert( clright );

	try {
		CLKernel kerncv( _stereoCVFBP_source, "stereoCV_FBP_AD" );
		CLKernel kerncvfbp( _stereoCVFBP_source, "stereoCV_FBP" );
		CLKernel kerncvwta( _stereoCVFBP_source, "stereoCV_FBP_WTA" );
		CLKernel kerncvsobel( _stereoCVFBP_source, "stereoCV_FBP_Sobel" );
//		CLKernel kerncvup( _stereoCVFBP_source, "stereoCV_FBP_HALFDOWN" );
//		CLKernel kerncvdown( _stereoCVFBP_source, "stereoCV_FBP_HALFUP" );

	//	Image tmp2( clleft, IALLOCATOR_CL );
	//	kerncvsobel.setArg( 0, clleft );
	//	kerncvsobel.setArg( 1, tmp2 );
	//	kerncvsobel.run( CLNDRange( Math::pad( left.width(), 16 ), Math::pad( left.height(), 16 ) ), CLNDRange( 16, 16 ) );
	//	clleft.save("sobell.png");

	//	Image tmp3( clright, IALLOCATOR_CL );
	//	kerncvsobel.setArg( 0, clright );
	//	kerncvsobel.setArg( 1, tmp3 );
	//	kerncvsobel.run( CLNDRange( Math::pad( left.width(), 16 ), Math::pad( left.height(), 16 ) ), CLNDRange( 16, 16 ) );
	//	clright.save("sobelr.png");

		CLBuffer cv( sizeof( cl_float2 ) * left.width() * left.height() * depth  );
//		CLBuffer cv2( sizeof( cl_float2 ) * ( left.width() / 2 )  * ( left.height() / 2 ) * depth  );
//		CLBuffer cv3( sizeof( cl_float2 ) * ( left.width() / 4 )  * ( left.height() / 4 ) * depth  );

		Time t;

		kerncv.setArg( 0, cv );
		kerncv.setArg( 1, depth );
		kerncv.setArg( 2, clright );
		kerncv.setArg( 3, clleft );
		kerncv.setArg( 4, CLLocalSpace( sizeof( cl_float ) * 1 * ( depth + 256 ) ) );
		kerncv.run( CLNDRange( Math::pad( left.width(), 256 ), left.height() ), CLNDRange( 256, 1 ) );

#if 0
		kerncvdown.setArg( 0, cv2 );
		kerncvdown.setArg( 1, cv );
		kerncvdown.setArg<int>( 2, ( left.width() / 2 ) );
		kerncvdown.setArg<int>( 3, ( left.height() / 2 ) );
		kerncvdown.setArg( 4, depth );
		kerncvdown.run( CLNDRange( Math::pad( left.width() / 2, 16 ), Math::pad( left.height() / 2, 16 ), depth ), CLNDRange( 16, 16, 1 ) );


		for( int i = 0; i < iter; i++ ) {
			kerncvfbp.setArg( 0, cv2 );
			kerncvfbp.setArg<int>( 1, left.width() / 2 );
			kerncvfbp.setArg<int>( 2, left.height() / 2 );
			kerncvfbp.setArg( 3, depth );
			kerncvfbp.setArg( 4, i );
			kerncvfbp.run( CLNDRange( Math::pad( left.width() / 4, 16 ), Math::pad( left.height(), 16 ) ), CLNDRange( 16, 16 ) );
		}

		kerncvup.setArg( 0, cv );
		kerncvup.setArg( 1, cv2 );
		kerncvup.setArg<int>( 2, left.width() );
		kerncvup.setArg<int>( 3, left.height() );
		kerncvup.setArg( 4, depth );
		kerncvup.run( CLNDRange( Math::pad( left.width(), 16 ), Math::pad( left.height(), 16 ), depth ), CLNDRange( 16, 16, 1 ) );
#endif

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

		Image tmp;
		std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;
		ColorCode::apply( tmp, cldmap, 1.0f, 0.0f );
		tmp.save("dmap-fbp-color.png");
		cldmap.save( "dmap-fbp.png" );

	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
