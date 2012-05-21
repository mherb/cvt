#include <cvt/io/VideoReader.h>
#include <cvt/io/RawVideoReader.h>
#include <cvt/io/RawVideoWriter.h>
#include <cvt/io/Resources.h>
#include <cvt/util/Time.h>
#include <cvt/util/Exception.h>

using namespace cvt;

int main( int argc, char** argv )
{
	if( argc < 4 ){
		std::cout << "Usage: " << argv[ 0 ] << " <file-in> <startIdx> <stopIdx>" << std::endl;
		return 0;
	}
	
	String input( argv[ 1 ] );
	String output( input );
	output += ".cutted";
	int start = atoi( argv[ 2 ] );
	int stop  = atoi( argv[ 3 ] );

	try {
		RawVideoReader video( input );

		int iter = 0;
		std::cout << "Searching start position ..." << std::endl;
		while( iter < start ){
			video.nextFrame();
			iter++;
		}
		std::cout << "Copying ..." << std::endl;
		
		RawVideoWriter writer( output );
		while( iter < stop ){
			writer.write( video.frame() );
			video.nextFrame();
			iter++;
		}
	} catch( cvt::Exception e ) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
