#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/ImageIO.h>
#include <string>
#include <iostream>

#include "fill.h"

using namespace cvt;

int main( int argc, char** argv )
{
	CLContext cl;
	std::string log;

	try {

	cl.makeCurrent();

	CLKernel kernel;
	kernel.build("fill", _fill_source, strlen( _fill_source ), log );

	Image x( 400, 300, IOrder::RGBA, IType::UBYTE, IALLOCATOR_CL );

	kernel.setArg( 0, &x );

	kernel.run( cl::NullRange, cl::NDRange( 400, 300 ), cl::NullRange );

	ImageIO::savePNG( x, "bla.png" );
	} catch( CLException e ) {
		std::cout << e.what() << std::endl;
	}

}
