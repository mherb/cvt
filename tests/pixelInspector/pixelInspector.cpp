#include <cvt/gfx/Image.h>
#include <cvt/io/Resources.h>

using namespace cvt;

int main( int argc, char * argv[ 0 ] )
{
	if( argc < 4 ){
		std::cout << "Usage: ./pixelInspector <imagefile> <xpos> <ypos> [<patchsize=7>]" << std::endl;
	}

	String tmp;
	
	tmp = argv[ 2 ];
	int x = tmp.toInteger();
	tmp = argv[ 3 ];
	int y = tmp.toInteger();

	int patchSize = 7;
	if( argc == 5 ){
		tmp = argv[ 4 ];
		patchSize = tmp.toInteger();
		if( patchSize < 0 ){
			std::cerr << "NEGATIVE PATCHSIZE NOT ALLOWED" << std::endl;
		}
	}
	
	try {
		Image img;
		img.load( argv[ 1 ] );

		Image gray;
		img.convert( gray, IFormat::GRAY_UINT8 );

		size_t stride;
		uint8_t * ptr = gray.map( &stride );

		size_t patchHalfSize = patchSize >> 1;
		for( size_t h = y-patchHalfSize; h <= y + patchHalfSize; h++ ){
			for( size_t w = x-patchHalfSize; w <= x + patchHalfSize; w++ ){
				std::cout << std::setw( 3 ) << (int)*( ptr + h * stride + w ) << " ";
			}
			std::cout << std::endl;
		}

		gray.unmap( ptr );
		

	} catch ( const Exception & e )
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
