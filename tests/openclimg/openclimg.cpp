#include <cvt/cl/CLContext.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/GaussIIR.h>
#include <cvt/io/ImageIO.h>
#include <cvt/io/Resources.h>
#include <cvt/util/Time.h>

#include <iostream>


using namespace cvt;

int main( void )
{
	CLContext cl;
	Resources resources;
	GaussIIR filter;

	ParamSet * pSet = filter.parameterSet();

	size_t in = pSet->paramHandle( "Input" );
	size_t out = pSet->paramHandle( "Output" );
	size_t sigma = pSet->paramHandle( "Sigma" );
	size_t order = pSet->paramHandle( "Order" );

	try {
		cl.makeCurrent();

		Image* tmp = new Image();
		ImageIO::loadPNG( *tmp, resources.find( "lena.png" ) );

		Image x( *tmp, IALLOCATOR_CL );
		delete tmp;

		Image x2;
		x2.reallocate( x, IALLOCATOR_CL );

		pSet->setArg( in, &x );
		pSet->setArg( out, &x2 );
		pSet->setArg( sigma, 2.5f );
		pSet->setArg<int>( order, 0 );

		filter.apply( pSet, IFILTER_OPENCL );
		ImageIO::savePNG( x2, "output-cl.png" );
		filter.apply( pSet, IFILTER_CPU );
		ImageIO::savePNG( x2, "output-cpu.png" );

	} catch( CLException e ) {
		std::cout << e.what() << std::endl;
	} catch( Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
