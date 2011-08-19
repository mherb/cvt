#include <iostream>
#include <CLPlatform.h>
#include <CLContext.h>
#include <CLCommandQueue.h>
#include <CLBuffer.h>
#include <CLNDRange.h>
#include <CLProgram.h>
#include <CLKernel.h>
#include <CLImage2D.h>

using namespace cvt;

const char* progsource = "__kernel void TESTKERN( __global float* dst, __global float* src, float val )"\
					"{" \
					" int x = get_global_id( 0 );" \
					" dst[ x ] = src[ x ] * val;" \
					"}";

int main()
{
	std::string str;
	std::vector<CLPlatform> platforms;
	std::vector<CLDevice> devices;


	CLPlatform::get( platforms );
	std::cout << platforms[ 0 ] << std::endl;



	platforms[ 0 ].devices( devices );
	for( size_t i = 0; i < devices.size(); i++ )
		std::cout << devices[ i ] << std::endl;

/*
	std::cout << "\n\n\n\n\n\n\n\n\n";

	CLContext cl( platforms[ 0 ], CL_DEVICE_TYPE_ALL, true );

	CLContext cl2( platforms[ 0 ], devices, true );

	cl.devices( devices );
	for( int i = 0; i < devices.size(); i++ )
		std::cout << devices[ i ] << std::endl;

	std::cout << cl.platform() << std::endl;*/


	CLContext cl( platforms[ 0 ], CL_DEVICE_TYPE_ALL, true );

	std::vector<CLImageFormat> iformats;
	cl.supportedImage2DFormats( iformats );
	for( size_t i = 0; i < iformats.size(); i++ )
		std::cout << iformats[ i ] << std::endl;

	CLCommandQueue q( cl, cl.defaultDevice() );

	float values[ 10 ];
	float val[ 10 ];

	for( int i = 0; i < 10; i++ ) {
		values[ i ] = ( float ) i;
		val[ i ] = 0;
	}

	CLBuffer buf1( cl, sizeof( float ) * 10 );
	CLBuffer buf2( cl, sizeof( float ) * 10 );
	q.enqueueWriteBuffer( buf1, values, sizeof( float ) * 10 );
	q.enqueueCopyBuffer( buf2, buf1, sizeof( float ) * 10 );
	q.enqueueReadBuffer( buf2, val, sizeof( float ) * 10 );

	for( int i = 0; i < 10; i++ )
		std::cout << val[ i ] << std::endl;

	CLProgram prog( cl, progsource );
	bool b = prog.build( cl.defaultDevice() );
	if( !b ) {
		std::cout << "Compile failed" << std::endl;
		std::string log;
		prog.buildLog( cl.defaultDevice(), log );
		std::cout << "Log: \n" << log << std::endl;
	}

	CLKernel kern( prog, "TESTKERN" );
	std::cout << kern << std::endl;

	kern.setArg( 0, buf2 );
	kern.setArg( 1, buf1 );
	kern.setArg( 2, 2.0f );

	q.enqueueNDRangeKernel( kern, CLNDRange( 10 ) );

	q.enqueueReadBuffer( buf2, val, sizeof( float ) * 10 );
	std::cout << "Result:\n";
	for( int i = 0; i < 10; i++ )
		std::cout << val[ i ] << std::endl;

	CLEvent event;
	q.enqueueNDRangeKernel( kern, CLNDRange( 10 ), CLNDRange(), CLNDRange(), NULL, &event );
	q.waitEvent( event );

	CLImage2D img( cl, 640, 480, CLImageFormat( CL_RGBA, CL_UNORM_INT8 ) );

}
