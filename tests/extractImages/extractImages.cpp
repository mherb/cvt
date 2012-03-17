#include <cvt/io/RawVideoReader.h>
#include <cvt/util/String.h>

using namespace cvt;

int main( int argc, char* argv[] )
{
	if( argc < 3 ){
		std::cout << "Usage: " << argv[ 0 ] << " <rawvidfile> <base_name>" << std::endl;
		return 0;
	}

	String file( argv[ 1 ] );
	String base( argv[ 2 ] );

	RawVideoReader vid( file, false );
	
	Image imgOut;
	String saveName;

	size_t printStep = vid.numFrames() / 40;

	for( size_t i = 0; i < vid.numFrames(); i++ ){
		vid.frame().convert( imgOut, IFormat::RGBA_UINT8 );
		saveName.sprintf( "%s_%06d.png", base.c_str(), i );
		imgOut.save( saveName );
		vid.nextFrame();


		if( i % printStep == 0 ){
			std::cout  << "#";
			std::flush( std::cout );
		}
	}
	std::cout << std::endl;

	return 0;
}
