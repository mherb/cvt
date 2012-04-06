#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLBuffer.h>
#include <cvt/util/String.h>
#include <cvt/util/Time.h>

#include <cvt/cl/kernel/fgp/fgpgrayweightedhuber.h>
#include <cvt/cl/kernel/fgp/fgpgrayweightedhuber_data.h>
#include <cvt/cl/kernel/clear.h>
#include <cvt/cl/kernel/stereo/stereoCV.h>
#include <cvt/cl/kernel/stereo/stereoCVQSearch.h>
#include <cvt/cl/kernel/imageutil.h>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 4 ) {
		std::cout << "usage: " << argv[ 0 ] << " left right dmax" << std::endl;
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
	Image cldmap( left.width(), left.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
	Image cldmaptmp( left.width(), left.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
	Image output( left.width(), left.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );

	try {
		float theta = 0.5f;
		float lambda = 1.0f;
		float t, told;
		int inner = 40, outer = 100, n = 0, i = 0;
		CLKernel kernclear( _clear_source, "clear" );
		CLKernel kernfgp( _fgpgrayweightedhuber_source, "fgp" );
		CLKernel kernfgpdata( _fgpgrayweightedhuber_data_source, "fgp_data" );
		CLKernel kerncv( _stereoCV_source, "stereoCV" );
		CLKernel kerncvwta( _stereoCV_source, "stereoCV_WTAMINMAX" );
		CLKernel kerncvqsearch( _stereoCVQSearch_source, "stereoCV_QSearch" );
		CLKernel kernextractr( _imageutil_source, "image_rgba_to_gray_x" );
		CLKernel kernexpw( _imageutil_source, "image_gradexp_to_x" );

		CLBuffer cv( sizeof( cl_float ) * left.width() * left.height() * depth  );

		kerncv.setArg( 0, cv );
		kerncv.setArg( 1, depth );
		kerncv.setArg( 2, clright );
		kerncv.setArg( 3, clleft );
		kerncv.setArg( 4, CLLocalSpace( sizeof( cl_float4 ) * ( depth + 256 ) ) );
		kerncv.run( CLNDRange( Math::pad( left.width(), 256 ), left.height() ), CLNDRange( 256, 1 ) );

		kerncvwta.setArg( 0, cldmaptmp );
		kerncvwta.setArg( 1, cv );
		kerncvwta.setArg( 2, depth );
		kerncvwta.run( CLNDRange( Math::pad16( left.width() ), Math::pad16( left.height() ) ), CLNDRange( 16, 16 ) );

		kernexpw.setArg( 0, cldmap );
		kernexpw.setArg( 1, cldmaptmp );
		kernexpw.setArg( 2, clright );
		kernexpw.setArg( 3, 2 );
		kernexpw.run( CLNDRange( Math::pad16( left.width() ), Math::pad16( left.height() ) ), CLNDRange( 16, 16 ) );

		Image e0( cldmap.width(), cldmap.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image e1( cldmap.width(), cldmap.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );

		Time timer;

			told = t = 1.0f;
			kernclear.setArg( 0, e0 );
			kernclear.run( CLNDRange( Math::pad16( cldmap.width() ), Math::pad16( cldmap.height() ) ), CLNDRange( 16, 16 ) );


#define THETA 10.0f
#define LAMBDA 5.0f
#define BETA 0.00001f
#define KSIZE 16
//#define DEBUGIMAGE 1

		while( i < outer ) {
		//	theta = 0.01f;
			theta = THETA * ( exp( -6.0f * Math::sqr( ( ( float )  i ) / ( float ) ( outer - 1 ) ) ) ) + 1e-4f;
			lambda = LAMBDA * ( 1.0f -  ( ( float )  i ) / ( float ) ( outer - 1 ) ) + 0.5f;



			std::cout << "Theta: " << theta << std::endl;
			n = 0;
			while( n < inner ) {
				bool b = n & 0x01;
				kernfgp.setArg( 0,  b ? e0 : e1 );
				kernfgp.setArg( 1,  b ? e1 : e0 );
				kernfgp.setArg( 2, cldmap );
				kernfgp.setArg( 3, theta );
				kernfgp.setArg( 4, ( told - 1.0f ) / t );
				kernfgp.setArg( 5, CLLocalSpace( sizeof( cl_float4 ) * ( ( KSIZE + 2 ) * ( KSIZE + 2 ) ) ) );
				kernfgp.setArg( 6, CLLocalSpace( sizeof( cl_float4 ) * ( ( KSIZE + 1 ) * ( KSIZE + 1 ) ) ) );
				kernfgp.run( CLNDRange( Math::pad( cldmap.width(), KSIZE ), Math::pad( cldmap.height(), KSIZE ) ), CLNDRange( KSIZE, KSIZE ) );

				told = t;
				t = 0.5f * ( 1.0f + Math::sqrt( 1.0f + 4.0f * Math::sqr( told ) ) );
				n++;
			}

			kernfgpdata.setArg( 0, cldmaptmp );
			kernfgpdata.setArg( 1, ( n & 0x01 ) ? e1 : e0 );
			kernfgpdata.setArg( 2, cldmap );
			kernfgpdata.setArg( 3, theta );
			kernfgpdata.setArg( 4, ( told - 1.0f ) / t );
			kernfgpdata.setArg( 5, CLLocalSpace( sizeof( cl_float4 ) * ( 17 * 17 ) ) );
			kernfgpdata.run( CLNDRange( Math::pad16( cldmap.width() ), Math::pad16( cldmap.height() ) ), CLNDRange( 16, 16 ) );

#ifdef DEBUGIMAGE
			{
				kernextractr.setArg( 0, output );
				kernextractr.setArg( 1, cldmaptmp );
				kernextractr.setArg( 2, 0 );
				kernextractr.run( CLNDRange( Math::pad16( cldmap.width() ), Math::pad16( cldmap.height() ) ), CLNDRange( 16, 16 ) );
				output.save( "dmapcvtvl1.png" );
				getchar();
			}
#endif

			std::cout << "Theta: " << theta << std::endl;
			kerncvqsearch.setArg( 0, cldmap );
			kerncvqsearch.setArg( 1, cldmaptmp );
			kerncvqsearch.setArg( 2, cv );
			kerncvqsearch.setArg( 3, depth );
			kerncvqsearch.setArg( 4, theta );
			kerncvqsearch.setArg( 5, lambda );
			kerncvqsearch.run( CLNDRange( Math::pad16( left.width() ), Math::pad16( left.height() ) ), CLNDRange( 16, 16 ) );

#ifdef DEBUGIMAGE
			{
				kernextractr.setArg( 0, output );
				kernextractr.setArg( 1, cldmap );
				kernextractr.setArg( 2, 0 );
				kernextractr.run( CLNDRange( Math::pad16( cldmap.width() ), Math::pad16( cldmap.height() ) ), CLNDRange( 16, 16 ) );
				output.save( "dmapcvtvl1.png" );
				getchar();
			}
#endif

			i++;
	//		theta = theta * ( 1.0f - BETA * i );
	//		theta = Math::max( 1e-4f, theta );
		}


		{
			kernextractr.setArg( 0, output );
			kernextractr.setArg( 1, cldmap );
			kernextractr.setArg( 2, 0 );
			kernextractr.runWait( CLNDRange( Math::pad16( cldmap.width() ), Math::pad16( cldmap.height() ) ), CLNDRange( 16, 16 ) );
			Image out;
			output.convert( out, IFormat::GRAY_UINT8 );
			out.save( "dmapcvtvl1.png" );
		}

		std::cout << timer.elapsedMilliSeconds() << " ms" << std::endl;

	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
