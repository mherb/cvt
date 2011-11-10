#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>

#include <iomanip>

#include <cvt/cl/kernel/prefixsum/prefixsum_block.h>
#include <cvt/cl/kernel/prefixsum/prefixsum_horiz.h>
#include <cvt/cl/kernel/prefixsum/prefixsum_vert.h>
#include <cvt/cl/kernel/prefixsum/prefixsum_block2.h>

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
	Image climg( input.width(), input.height(), input.format(), IALLOCATOR_CL );
	input.convert( climg );
//	climg.fill( Color::WHITE );

	Image output( input.width(), input.height(), IFormat::floatEquivalent( input.format() ), IALLOCATOR_CL );

	try {
		CLKernel kern( _prefixsum_block_source, "prefixsum_block" );
		kern.setArg( 0, output );
		kern.setArg( 1, climg );
		kern.setArg( 2, CLLocalSpace( sizeof( cl_float4 ) * 32 * 32 ) );
		kern.run( CLNDRange( Math::pad( input.width(), 32 ), Math::pad( input.height(), 32 ) >> 5 ), CLNDRange( 32, 1 ) );


		CLKernel kern2( _prefixsum_horiz_source, "prefixsum_horiz" );
		kern2.setArg( 0, output );
		kern2.setArg( 1, output );
		kern2.setArg<int>( 2, 32 );
		kern2.run( CLNDRange( Math::pad( input.height(), 32 ) ), CLNDRange( 32 ) );

		CLKernel kern3( _prefixsum_vert_source, "prefixsum_vert" );
		kern3.setArg( 0, output );
		kern3.setArg( 1, output );
		kern3.setArg<int>( 2, 32 );
		kern3.run( CLNDRange( Math::pad( input.width(), 32 ) ), CLNDRange( 32 ) );

		CLKernel kern4( _prefixsum_block2_source, "prefixsum_block2" );
		kern4.setArg( 0, output );
		kern4.setArg( 1, output );
		kern4.setArg( 2, CLLocalSpace( sizeof( cl_float4 ) * 32 ) );
		kern4.setArg( 3, CLLocalSpace( sizeof( cl_float4 ) * 32 ) );
		kern4.run( CLNDRange( Math::pad( input.width(), 32 ), Math::pad( input.height(), 32 ) ), CLNDRange( 32, 32 ) );

		output.save( "prefixsum.png" );

		Image iicpu( input.width(), input.height(), IFormat::floatEquivalent( input.format() ) );
		Image bla( input );
		bla.integralImage( iicpu );
		iicpu.mul( 1.0f / 255.0f );
		std::cout << "SSD:"  << iicpu.ssd( output ) / ( float )( input.width() * input.height() ) << std::endl;
		iicpu.sub( output );
		iicpu.save( "bla.png" );
	//	climg.integralImage( iicpu );
#if 0
		{
			size_t stride;
			float* data = output.map<float>( &stride );
			float* dptr = data;
			for( int y = 0; y < 96; y++ ) {
				for( int x = 0; x < 96; x++ ) {
					std::cout << std::setw( 6 ) << std::setprecision( 5 ) << dptr[ x ] << " ";
				}
				std::cout << std::endl;
				dptr += stride;
			}
			output.unmap( data );
		}


		{
			size_t stride;
			float* data = iicpu.map<float>( &stride );
			float* dptr = data;
			for( int y = 0; y < 96; y++ ) {
				for( int x = 0; x < 96; x++ ) {
					std::cout << std::setw( 6 ) << std::setprecision( 5 ) << dptr[ x ] << " ";
				}
				std::cout << std::endl;
				dptr += stride;
			}
			iicpu.unmap( data );
		}
#endif
	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}


	return 0;
}
