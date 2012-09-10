#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/Threshold.h>
#include <cvt/gfx/ifilter/MorphErode.h>
#include <cvt/gfx/ifilter/MorphDilate.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/gfx/ifilter/GuidedFilter.h>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 2 ) {
		std::cout << "usage: " << argv[ 0 ] << " image" << std::endl;
	}
	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );


	Image tmp, gray;
	tmp.load( argv[ 1 ] );
	tmp.convert( gray, IFormat::GRAY_UINT8 );

	Threshold thres;
	thres.apply( tmp, gray, 0.01f );

	tmp.save("out.png");

	MorphErode erode;
	erode.apply( gray, tmp, 10 );

	MorphDilate dilate;
	dilate.apply( tmp, gray, 10 );

	tmp.save("out2.png");

	GuidedFilter gf;
    Image i0, i1, tmp2;
	tmp2.load( argv[ 1 ] );
	tmp2.convert( i0, IFormat::RGBA_UINT8 );
	tmp.convert( i1, IFormat::GRAY_UINT8 );

	Image img0( i0, IALLOCATOR_CL );
	Image img1( i1, IALLOCATOR_CL );
	Image out( i0.width(), i0.height(), IFormat::RGBA_UINT8, IALLOCATOR_CL );

	gf.apply( out, img1, img0, 40, 1e-3, true );

	out.save( "outmatte.png" );

}
