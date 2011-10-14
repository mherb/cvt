#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/gfx/Image.h>

#include <iostream>

#include "BHist.h"

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
	Image climg( input.width(), input.height(), IFormat::GRAY_FLOAT, IALLOCATOR_CL );
	input.convert( climg );

#define LENGTH 10
	CLKernel kern( _BHist_source, "bhist" );
	CLBuffer clhist( sizeof( cl_uint ) * ( LENGTH + 1 ) );
	CLBuffer cljointhist( sizeof( cl_uint ) * ( LENGTH + 1 ) * ( LENGTH + 1 ) );

	cl_uint* cluptr = ( cl_uint* ) clhist.map();
	for( int i = 0; i < LENGTH + 1; i++ )
		cluptr[ i ] = 0;
	clhist.unmap( ( void* ) cluptr );

	cluptr = ( cl_uint* ) cljointhist.map();
	for( int i = 0; i < ( LENGTH + 1 ) * ( LENGTH + 1 ); i++ )
		cluptr[ i ] = 0;
	cljointhist.unmap( ( void* ) cluptr );


	kern.setArg( 0, clhist );
	kern.setArg( 1, cljointhist );
	kern.setArg<cl_int>( 2, LENGTH );
	kern.setArg( 3, climg );
	kern.setArg( 4, climg );
	kern.setArg( 5, CLLocalSpace( sizeof( float ) * ( LENGTH + 1 ) * 16 ) );
	kern.setArg( 6, CLLocalSpace( sizeof( float ) * ( LENGTH + 1 ) * ( LENGTH + 1 ) * 16 ) );
	kern.run( CLNDRange( climg.width() / 4, climg.height() / 4 ), CLNDRange( 4, 4 ) );

	cluptr = ( cl_uint* ) clhist.map();
	for( int i = 0; i < LENGTH; i++ )
		std::cout << i << " " << ( float ) cluptr[ i ] / ( float ) ( 0x100 * climg.width() * climg.height() ) << std::endl;
	clhist.unmap( ( void* ) cluptr );
}


