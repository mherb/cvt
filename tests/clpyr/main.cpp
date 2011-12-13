#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>

#include <cvt/util/Time.h>

#include <iomanip>

#include <cvt/cl/kernel/pyr/pyrdown_binom3.h>
#include <cvt/cl/kernel/pyr/pyrdown_binom5.h>
#include <cvt/cl/kernel/pyr/pyrup.h>

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

/*	std::cout << platforms[ 0 ] << std::endl;
	std::cout << platforms[ 0 ].defaultDevice() << std::endl;
	std::vector<CLImageFormat> fmts;
	CL::defaultContext()->supportedImage2DFormats( fmts );
	for( int i = 0; i < fmts.size(); i++ )
		std::cout << fmts[ i ] << std::endl;
*/
	Image input( argv[ 1 ] );

	input.save("pyrorig.png");

	try {

			CLKernel kern( _pyrdown_binom3_source, "pyrdown_binom3" );
			CLKernel kernup( _pyrup_source, "pyrup" );

			Image climg( input.width(), input.height(), input.format(), IALLOCATOR_CL );
			input.convert( climg );
			//climg.fill( Color::WHITE );
#define CLSIZE 16
#define SCALE 0.75f
			Image output( input.width() * SCALE, input.height() * SCALE, input.format(), IALLOCATOR_CL );


			Time t;
#define BLUR 1
#ifdef BLUR
			kern.setArg( 0, output );
			kern.setArg( 1, climg );
			kern.setArg( 2, CLLocalSpace( sizeof( cl_float4 ) * ( CLSIZE + 4 ) * ( CLSIZE + 4 ) ) );
			kern.setArg( 3, CLLocalSpace( sizeof( cl_float4 ) * ( CLSIZE + 4 ) * CLSIZE ) );
			kern.runWait( CLNDRange( Math::pad( output.width(), CLSIZE ), Math::pad( output.height(), CLSIZE ) ), CLNDRange( CLSIZE, CLSIZE ) );
#else
			kernup.setArg( 0, output );
			kernup.setArg( 1, climg );
			kernup.runWait( CLNDRange( Math::pad( output.width(), CLSIZE ), Math::pad( output.height(), CLSIZE ) ), CLNDRange( CLSIZE, CLSIZE ) );
#endif

			std::cout << output.width() * output.height() << " " << t.elapsedMilliSeconds() << std::endl;
			output.save( "pyrdown.png" );

			t.reset();

			kernup.setArg( 0, climg );
			kernup.setArg( 1, output );
			kernup.runWait( CLNDRange( Math::pad( input.width(), CLSIZE ), Math::pad( input.height(), CLSIZE ) ), CLNDRange( CLSIZE, CLSIZE ) );
			std::cout << input.width() * input.height() << " " << t.elapsedMilliSeconds() << std::endl;

			climg.save("pyrup.png");


	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
