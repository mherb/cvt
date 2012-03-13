#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLBuffer.h>
#include <cvt/util/String.h>
#include <cvt/util/Time.h>

#include <cvt/cl/kernel/TSDFVolume/TSDFVolume.h>

#define VOL_WIDTH  512
#define VOL_HEIGHT 512
#define VOL_DEPTH  512

using namespace cvt;

int main( int argc, char** argv )
{
	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

	try {
		CLKernel kernclr( _TSDFVolume_source, "TSDFVolume_clear" );
		CLKernel kernadd( _TSDFVolume_source, "TSDFVolume_add" );

		CLBuffer cv( sizeof( cl_float2 ) * VOL_WIDTH * VOL_HEIGHT * VOL_DEPTH  );

		Time t;

		kernclr.setArg( 0, cv );
		kernclr.setArg( 1, VOL_WIDTH );
		kernclr.setArg( 2, VOL_HEIGHT );
		kernclr.setArg( 3, VOL_DEPTH );
		kernclr.run( CLNDRange( Math::pad16( VOL_WIDTH ), Math::pad16( VOL_HEIGHT ), VOL_DEPTH ), CLNDRange( 16, 16, 1 ) );

		/*
		kerncvwta.setArg( 0, cldmap );
		kerncvwta.setArg( 1, cv );
		kerncvwta.setArg( 2, depth );
		kerncvwta.runWait( CLNDRange( Math::pad16( left.width() ), Math::pad16( left.height() ) ), CLNDRange( 16, 16 ) );*/

		std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;


	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
