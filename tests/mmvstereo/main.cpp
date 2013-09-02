#include <cvt/io/MMVStereoParser.h>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc != 3 ) {
		std::cout << "usage: " << argv[ 0 ] << " paramfile image-path" << std::endl;
		return 0;
	}

	MMVStereoParser mmvdata( argv[ 1 ], argv[ 2 ] );
	std::cout << "Got " << mmvdata.size() << " images" << std::endl;

	for( size_t i = 0; i < mmvdata.size(); i++ )
	{
		std::cout << mmvdata[ i ].imagePath() << std::endl;
	}
}
