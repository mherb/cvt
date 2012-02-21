#include <cvt/gfx/Image.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLContext.h>

#include <cvt/util/Time.h>

#include <cvt/gfx/ifilter/TVL1Stereo.h>
#include <cvt/gfx/ifilter/ColorCode.h>
#include <cvt/gfx/ifilter/ROFFGPFilter.h>
#include <cvt/gfx/ifilter/GuidedFilter.h>

using namespace cvt;


int main( int argc, char** argv )
{
	if( argc != 3 && argc != 4 ) {
		std::cout << "usage: " << argv[ 0 ] << " image1 image2" << std::endl;
		return 0;
	}

	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

	Image input1( argv[ 1 ] );
	Image input2( argv[ 2 ] );

	Image output;
	Image ccode;

	try {
		TVL1Stereo stereo( 0.80, 20 );

		{
			Image in1, in2;
			input1.convert( in1, IFormat::RGBA_UINT8, IALLOCATOR_CL );
			input2.convert( in2, IFormat::RGBA_UINT8, IALLOCATOR_CL );
			Image tmp;

			ROFFGPFilter rof;
			GuidedFilter gf;

//			gf.apply( tmp, in1, in1, 60, 1e-1f );
//			rof.apply( tmp, in1, 0.5f, 200 );
//			tmp.save("smooth1.png");
//			in1.mad( tmp, -0.65f );

//			gf.apply( tmp, in2, in2, 60, 1e-1f );
//			rof.apply( tmp, in2, 0.5f, 200 );
//			tmp.save("smooth2.png");
//			in2.mad( tmp, -0.65f );

//			in1.save("structure1.png");
//			in2.save("structure2.png");

			Time t;
			stereo.apply( output, in1, in2 );


			std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;

		}
		Image tmp;
		ColorCode::apply( tmp, output, -80.0f, 10.0f );
		tmp.save("stereo.png");

//		output.add( -10.0f );
//		output.mul( -1.0f / 100.0f );
//		output.save("stereo.png");


	} catch( CLException& e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
