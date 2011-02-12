#include <cvt/io/VideoReader.h>
#include <cvt/io/Resources.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>

using namespace cvt;

int main( int argc, char** argv )
{
	size_t frames = 0;
	Time timer;
	
	if( argc < 2 ){
		std::cout << "Usage: " << argv[ 0 ] << " <filename>" << std::endl;
		return 0;
	}
	
	std::string xxx( argv[ 1 ] );

	try {
		VideoReader video( xxx );
		Image frame( video.width(), video.height(), IFormat::BGRA_UINT8 );

		timer.reset();
		while( 1 ) {
//		for(size_t i = 0; i < 300; i++){
			video.nextFrame();
			video.frame().convert( frame );

			frames++;
			if( timer.elapsedSeconds() > 5.0f ) {
				std::cout << "FPS: " << ( double ) frames / timer.elapsedSeconds() << std::endl;
				frames = 0;
				timer.reset();
			}
		}
	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
