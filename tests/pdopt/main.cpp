#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>

#include <cvt/util/Time.h>

#include <cvt/vision/Flow.h>
#include <cvt/io/FloFile.h>

#include <cvt/util/Time.h>

#include <cvt/cl/kernel/PDROF.h>
#include <cvt/cl/kernel/PDHuber.h>
#include <cvt/cl/kernel/fill.h>


using namespace cvt;

#define KX 16
#define KY 16
#define VIEWSAMPLES 3

int main( int argc, char** argv )
{
	if( argc != 2 ) {
		std::cout << "usage: " << argv[ 0 ] << " image1" << std::endl;
		return 0;
	}

	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

	Image tmp( argv[ 1 ] );
	Image input;
	tmp.convert( input, IFormat::floatEquivalent( tmp.format() ) );
	try {

		Image clinput( input, IALLOCATOR_CL );
		Image clout1( input.width(), input.height(), input.format(), IALLOCATOR_CL );
		Image clout2( input, IALLOCATOR_CL );
		Image clp1( input.width()*2, input.height(), input.format(), IALLOCATOR_CL );
		Image clp2( input.width()*2, input.height(), input.format(), IALLOCATOR_CL );

		CLKernel fill(_fill_source, "fill" );
		CLKernel pdrof(_PDHuber_source, "PDHuber" );

		Time timer;
		cl_float4 fillvalue = {{ 0.0f, 0.0f, 0.0f, 0.0f }};
		float lambda = 10.0f;

		fill.setArg( 0, clp2 );
		fill.setArg( 1, fillvalue );
		fill.run( CLNDRange( Math::pad( clp2.width(), 16 ), Math::pad( clp2.height(), 16 ) ), CLNDRange( 16, 16 ) );

		for( int i = 0; i < 50; i++ ) {
			pdrof.setArg( 0, clout1 );
			pdrof.setArg( 1, clp1 );
			pdrof.setArg( 2, clout2 );
			pdrof.setArg( 3, clp2 );
			pdrof.setArg( 4, clinput );
			pdrof.setArg( 5, lambda );
			pdrof.setArg( 6, CLLocalSpace( sizeof( cl_float4 ) * 18 * 18 ) );
			pdrof.setArg( 7, CLLocalSpace( sizeof( cl_float8 ) * 18 * 18 ) );
			pdrof.run( CLNDRange( Math::pad( clinput.width(), 16 ), Math::pad( clinput.height(), 16 ) ), CLNDRange( 16, 16 ) );

			pdrof.setArg( 0, clout2 );
			pdrof.setArg( 1, clp2 );
			pdrof.setArg( 2, clout1 );
			pdrof.setArg( 3, clp1 );
			pdrof.setArg( 4, clinput );
			pdrof.setArg( 5, lambda );
			pdrof.setArg( 6, CLLocalSpace( sizeof( cl_float4 ) * 18 * 18 ) );
			pdrof.setArg( 7, CLLocalSpace( sizeof( cl_float8 ) * 18 * 18 ) );
			pdrof.run( CLNDRange( Math::pad( clinput.width(), 16 ), Math::pad( clinput.height(), 16 ) ), CLNDRange( 16, 16 ) );
		}
		pdrof.setArg( 0, clout1 );
		pdrof.setArg( 1, clp1 );
		pdrof.setArg( 2, clout2 );
		pdrof.setArg( 3, clp2 );
		pdrof.setArg( 4, clinput );
		pdrof.setArg( 5, lambda );
		pdrof.setArg( 6, CLLocalSpace( sizeof( cl_float4 ) * 18 * 18 ) );
		pdrof.setArg( 7, CLLocalSpace( sizeof( cl_float8 ) * 18 * 18 ) );
		pdrof.runWait( CLNDRange( Math::pad( clinput.width(), 16 ), Math::pad( clinput.height(), 16 ) ), CLNDRange( 16, 16 ) );

		std::cout << timer.elapsedMilliSeconds() << std::endl;
		clout1.save("pdopt.png");



	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
