#include <cvt/gfx/Image.h>
#include <cvt/util/Exception.h>
#include <cvt/util/String.h>

using namespace cvt;

void genSequence( const Image& img, const String& baseName )
{
	Image dst( img.width() * 2, img.height() * 2, img.format() );
	Recti r;
	r.width = img.width();
	r.height = img.height();
	int y = dst.height() / 4;

	String name;
	for( size_t x = 0; x < img.width(); x++ ){		
		dst.fill( Color::WHITE );
		dst.copyRect( x, y + 50.0f * Math::cos( ( ( float ) x / ( float ) img.width() ) * Math::TWO_PI ), img, r );

		name.sprintf( "%s_%05d.png", baseName.c_str(), x );
		dst.save( name );
	}

}

int main( int argc, char* argv[] )
{
	if( argc < 3 ){
		std::cout << "Usage: " << argv[ 0 ] << " <templateimage> <outputbase>" << std::endl;
		return 0;
	}

	String baseName( argv[ 2 ] );

	Image img;
	try {
		img.load( argv[ 1 ] );
		genSequence( img, baseName );
	} catch( Exception& e ){
		std::cerr << e.what() << std::endl; 
	}

	return 0;
}
