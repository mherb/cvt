#include <cvt/gfx/Image.h>
#include <cvt/gfx/ifilter/Threshold.h>
#include <cvt/gfx/ifilter/MorphErode.h>
#include <cvt/gfx/ifilter/MorphDilate.h>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 2 ) {
		std::cout << "usage: " << argv[ 0 ] << " image" << std::endl;
	}

	Image tmp, gray;
	tmp.load( argv[ 1 ] );
	tmp.convert( gray, IFormat::GRAY_UINT8 );

	Threshold thres;
	thres.apply( tmp, gray, 0.005f );

	tmp.save("out.png");

	MorphErode erode;
	erode.apply( gray, tmp, 8 );

	MorphDilate dilate;
	dilate.apply( tmp, gray, 8 );


	tmp.save("out2.png");

}
