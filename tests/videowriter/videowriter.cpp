#include <cvt/io/RawVideoWriter.h>
#include <cvt/util/Time.h>

using namespace cvt;

int main( int argc, char* argv[] )
{
	if( argc < 2 ){
		std::cout << "Usage: " << argv[ 0 ] << " <img>" << std::endl;
		return 1;
	}

	Image img( argv[ 1 ] );

	try {
		RawVideoWriter writer( "test.rawvid" );

		int i = 0;
		Time t;
		while( i++ < 500 ){
			writer.write( img );
		}

		std::cout << "Avg. Time / frame: " << t.elapsedMilliSeconds() / ( 1000 * float (i) ) << std::endl;
	} catch ( const Exception & e ){
		std::cout << e.what() << std::endl;
	}

	return 0;
}
