#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/GaussIIR.h>
#include <cvt/gfx/ifilter/BrightnessContrast.h>
#include <cvt/io/Resources.h>
#include <cvt/util/Time.h>

#include <iostream>


using namespace cvt;

int main( void )
{
	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );



	Resources resources;
	GaussIIR filter;

	ParamSet * pSet = filter.parameterSet();

	size_t in = pSet->paramHandle( "Input" );
	size_t out = pSet->paramHandle( "Output" );
	size_t sigma = pSet->paramHandle( "Sigma" );
	size_t order = pSet->paramHandle( "Order" );

//	size_t contr = pSet->paramHandle( "Contrast" );
//	size_t bright = pSet->paramHandle( "Brightness" );

	try {

		Image* tmp = new Image();
		tmp->load( resources.find( "lena.png" ).c_str() );

		Image x( *tmp, IALLOCATOR_CL );
		delete tmp;

		Image x2;
		x2.reallocate( x, IALLOCATOR_CL );

		pSet->setArg( in, &x );
		pSet->setArg( out, &x2 );
//		pSet->setArg( contr, 0.5f );
		pSet->setArg( sigma, 2.5f );
		pSet->setArg<int>( order, 0 );


		filter.apply( pSet, IFILTER_OPENCL );
		x2.save( "output-cl.png" );

//		filter.apply( pSet, IFILTER_CPU );
  //      x2.save( "output-cpu.png" );

	} catch( CLException e ) {
		std::cout << e.what() << std::endl;
	} catch( Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
