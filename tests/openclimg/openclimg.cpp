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
		pSet->setArg( sigma, 1.5f );
		pSet->setArg<int>( order, 0 );
		
		Time timer;
		timer.reset();
		int i;
		for( i = 0; i < 60; i++ ){
			filter.apply( pSet, IFILTER_OPENCL );			
		}
		std::cout << "Average Time: " << timer.elapsedMilliSeconds() / i << std::endl;
		std::cout << "Average FPS: " << i * 1000.0f / timer.elapsedMilliSeconds() << std::endl;
		

		ImageIO::savePNG( x2, "bla.png" );
	} catch( CLException e ) {
		std::cout << e.what() << std::endl;
	} catch( Exception e ) {
		std::cout << e.what() << std::endl;
	}
	
	return 0;
}
