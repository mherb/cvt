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

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 2 ) {
		std::cout << "usage: " << argv[ 0 ] << " image" << std::endl;
		return 0;
	}

	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

	Image input( argv[ 1 ] );
	Image gray;
	input.convert( gray, IFormat::GRAY_FLOAT, IALLOCATOR_CL );
	Image output( gray.width(), gray.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );

	try {
		float lambda = 0.05f;
		float t = 1.0f, told = 1.0f;
		int iter = 50, n = 0;
		CLKernel kernclear( _clear_source, "clear" );
		CLKernel kernfgp( _fgpgrayweightedhuber_source, "fgp" );
		CLKernel kernfgpdata( _fgpgrayweightedhuber_data_source, "fgp_data" );

		// FIXME: add another buffer to read and write only to one ...
		Image e0( gray.width(), gray.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );
		Image e1( gray.width(), gray.height(), IFormat::RGBA_FLOAT, IALLOCATOR_CL );

		Time timer;

		kernclear.setArg( 0, e0 );
		kernclear.run( CLNDRange( Math::pad16( gray.width() ), Math::pad16( gray.height() ) ), CLNDRange( 16, 16 ) );

#define KSIZE 16
		while( n < iter ) {
			bool b = n & 0x01;
			kernfgp.setArg( 0,  b ? e0 : e1 );
			kernfgp.setArg( 1,  b ? e1 : e0 );
			kernfgp.setArg( 2, gray );
			kernfgp.setArg( 3, lambda );
			kernfgp.setArg( 4, ( told - 1.0f ) / t );
			kernfgp.setArg( 5, CLLocalSpace( sizeof( cl_float4 ) * ( ( KSIZE + 2 ) * ( KSIZE + 2 ) ) ) );
			kernfgp.setArg( 6, CLLocalSpace( sizeof( cl_float4 ) * ( ( KSIZE + 1 ) * ( KSIZE + 1 ) ) ) );
			kernfgp.run( CLNDRange( Math::pad( gray.width(), KSIZE ), Math::pad( gray.height(), KSIZE ) ), CLNDRange( KSIZE, KSIZE ) );

			told = t;
			t = 0.5f * ( 1.0f + Math::sqrt( 1.0f + 4.0f * Math::sqr( told ) ) );
			n++;
		}

		kernfgpdata.setArg( 0, output );
		kernfgpdata.setArg( 1, ( n & 0x01 ) ? e1 : e0 );
		kernfgpdata.setArg( 2, gray );
		kernfgpdata.setArg( 3, lambda );
		kernfgpdata.setArg( 4, ( told - 1.0f ) / t );
		kernfgpdata.setArg( 5, CLLocalSpace( sizeof( cl_float4 ) * ( 17 * 17 ) ) );
		kernfgpdata.runWait( CLNDRange( Math::pad16( gray.width() ), Math::pad16( gray.height() ) ), CLNDRange( 16, 16 ) );

		std::cout << timer.elapsedMilliSeconds() << " ms" << std::endl;

		output.save( "fgpgrayhuber.png" );

	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
