#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/ImageIO.h>
#include <string>
#include <iostream>

#include "fill.h"
#include "gaussiir.h"

using namespace cvt;

int main( int argc, char** argv )
{
	CLContext cl;
	std::string log;

	try {

	cl.makeCurrent();

	CLKernel kernel, kerneliir;
	kernel.build("fill", _fill_source, strlen( _fill_source ), log );
	kerneliir.build("gaussiir", _gaussiir_source, strlen( _gaussiir_source ), log );

	Image x( 400, 300, IOrder::RGBA, IType::UBYTE, IALLOCATOR_CL );

	kernel.setArg( 0, &x );

	kernel.run( x, cl::NullRange );

	ImageIO::savePNG( x, "bla.png" );
	} catch( CLException e ) {
		std::cout << e.what() << std::endl;
	}

}
