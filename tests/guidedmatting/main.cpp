#include <iostream>

#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/gfx/ifilter/GuidedFilter.h>
#include <cvt/io/Resources.h>


using namespace cvt;


int main( int argc, char* argv[] )
{

	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

	GuidedFilter gf;
    Image i0, i1, tmp;
	tmp.load( argv[ 1 ] );
	tmp.convert( i0, IFormat::RGBA_UINT8 );
	tmp.load( argv[ 2 ] );
	tmp.convert( i1, IFormat::RGBA_UINT8 );

	Image img0( i0, IALLOCATOR_CL );
	Image img1( i1, IALLOCATOR_CL );
	Image out( i0.width(), i0.height(), IFormat::RGBA_UINT8, IALLOCATOR_CL );

	gf.apply( out, img1, img0, 60, 1e-9f, true );

	out.save( "matte.png" );

	return 0;
}
