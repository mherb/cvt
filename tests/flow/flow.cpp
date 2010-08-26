#include <iostream>
#include <cvt/gfx/Image.h>
#include <cvt/io/FloFile.h>
#include <cvt/vision/Flow.h>

using namespace cvt;

int main(int argc, char* argv[])
{
	if( argc != 3 ) {
	    std::cerr << "usage: " << argv[ 0 ] << "file1.flo file2.flo" << std::endl;
	    exit( 1 );
	}
	Image img1( 1, 1 );
	Image img2( 1, 1 );
	float aee, aae;

	FloFile::FloReadFile( img1, argv[ 1 ] );
	FloFile::FloReadFile( img2, argv[ 2 ] );


	aee = Flow::AEE( img1, img2 );
	aae = Flow::AAE( img1, img2 );

	std::cout << "AEE: " << aee << std::endl;
	std::cout << "AAE: " << aae << std::endl;

	return 0;
}
