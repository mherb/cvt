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
	CLBuffer buf( sizeof( float ) * LENGTH );

	float* ptr = ( float* ) buf.map();
	for( int i = 0; i < LENGTH; i++ )
		ptr[ i ] = 0;
	buf.unmap( ( void* ) ptr );

	kern.setArg( 0, buf );
	kern.setArg<int>( 1, LENGTH );
	kern.setArg( 2, climg );
	kern.setArg( 3, CLLocalSpace( LENGTH * sizeof( float ) ) );
	kern.run( CLNDRange( climg.width(), climg.height() ), CLNDRange( 16, 16 ) );

	ptr = ( float* ) buf.map();
	for( int i = 0; i < LENGTH; i++ )
		std::cout << i << " " << ptr[ i ] / ( float ) ( climg.width() * climg.height() ) << std::endl;
	buf.unmap( ( void* ) ptr );
}


