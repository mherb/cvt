#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>

#include <cvt/util/Time.h>

#include <iomanip>

#include <cvt/cl/kernel/prefixsum/prefixsum_pblock.h>
#include <cvt/cl/kernel/prefixsum/prefixsum_horiz.h>
#include <cvt/cl/kernel/prefixsum/prefixsum_vert.h>
#include <cvt/cl/kernel/prefixsum/prefixsum_block2.h>
#include <cvt/cl/kernel/prefixsum/prefixsum_boxfilter.h>

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

/*	std::cout << platforms[ 0 ] << std::endl;
	std::cout << platforms[ 0 ].defaultDevice() << std::endl;
	std::vector<CLImageFormat> fmts;
	CL::defaultContext()->supportedImage2DFormats( fmts );
	for( int i = 0; i < fmts.size(); i++ )
		std::cout << fmts[ i ] << std::endl;
*/
	Image input( argv[ 1 ] );

	try {

			CLKernel kern( _prefixsum_pblock_source, "prefixsum_pblock" );

			CLKernel kern2( _prefixsum_horiz_source, "prefixsum_horiz" );

			CLKernel kern3( _prefixsum_vert_source, "prefixsum_vert" );

			CLKernel kern4( _prefixsum_block2_source, "prefixsum_block2" );

			CLKernel kern5( _prefixsum_boxfilter_source, "prefixsum_boxfilter" );

//		for( int size = 64; size <= 4096; size++ ) {
			Image climg( input.width(), input.height(), input.format(), IALLOCATOR_CL );
			input.convert( climg );
			//climg.fill( Color::WHITE );
			Image output(input.width(), input.height(), IFormat::floatEquivalent( input.format() ), IALLOCATOR_CL );


#define CLSIZE 16
			Time t;
			kern.setArg( 0, output );
			kern.setArg( 1, climg );
			kern.setArg( 2, CLLocalSpace( sizeof( cl_float4 ) * CLSIZE * CLSIZE ) );
			kern.run( CLNDRange( Math::pad( input.width(), CLSIZE ), Math::pad( input.height(), CLSIZE ) ), CLNDRange( CLSIZE, CLSIZE ) );

			kern2.setArg( 0, output );
			kern2.setArg( 1, output );
			kern2.setArg<int>( 2, CLSIZE );
			kern2.run( CLNDRange( Math::pad( input.height(), CLSIZE ) ), CLNDRange( CLSIZE ) );

			kern3.setArg( 0, output );
			kern3.setArg( 1, output );
			kern3.setArg<int>( 2, CLSIZE );
			kern3.run( CLNDRange( Math::pad( input.width(), CLSIZE ) ), CLNDRange( CLSIZE ) );

			kern4.setArg( 0, output );
			kern4.setArg( 1, output );
			kern4.setArg( 2, CLLocalSpace( sizeof( cl_float4 ) * CLSIZE ) );
			kern4.setArg( 3, CLLocalSpace( sizeof( cl_float4 ) * CLSIZE ) );
			kern4.runWait( CLNDRange( Math::pad( input.width(), CLSIZE ), Math::pad( input.height(), CLSIZE ) ), CLNDRange( CLSIZE, CLSIZE ) );

			std::cout << input.width() * input.height() << " " << t.elapsedMilliSeconds() << std::endl;

			kern5.setArg( 0, climg );
			kern5.setArg( 1, output );
			kern5.setArg( 2, 8 );
			kern5.runWait( CLNDRange( Math::pad( input.width(), CLSIZE ), Math::pad( input.height(), CLSIZE ) ), CLNDRange( CLSIZE, CLSIZE ) );

			climg.save( "boxfilter.png" );
#if 1

			Image iicpu( input.width(), input.height(), IFormat::floatEquivalent( input.format() ) );
			input.integralImage( iicpu );
			iicpu.mul( 1.0f / 255.0f );
			std::cout << "SSD:"  << iicpu.ssd( output ) / ( float )( input.width() * input.height() ) << std::endl;
			output.sub( iicpu );
			output.save( "bla.png" );
			//	climg.integralImage( iicpu );
#endif

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

//		}
	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
