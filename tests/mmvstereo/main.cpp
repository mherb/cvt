#include <cvt/gui/Application.h>
#include <cvt/io/MMVStereoParser.h>
#include <cvt/gfx/Image.h>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 3 ) {
		std::cout << "usage: " << argv[ 0 ] << " paramfile image-path" << std::endl;
		return 0;
	}

	Application::init();
	MMVStereoParser mmvdata( argv[ 1 ], argv[ 2 ] );
	std::cout << "Got " << mmvdata.size() << " images" << std::endl;

/*	for( size_t i = 0; i < mmvdata.size(); i++ ) {
		std::cout << mmvdata[ i ].imagePath() << std::endl;
	}*/

	Image climg1( mmvdata[ 0 ].imagePath(), IALLOCATOR_CL );
	Image climg2( mmvdata[ 1 ].imagePath(), IALLOCATOR_CL );
	Image climg3( mmvdata[ 2 ].imagePath(), IALLOCATOR_CL );


}
