#include <cvt/gfx/Image.h>
#include <cvt/gfx/IMapScoped.h>

#include <cvt/gfx/IColorCode.h>

using namespace cvt;

int main()
{
	Image img( 800, 600, IFormat::GRAY_FLOAT );

	{
		IMapScoped<float> map( img );
		size_t h = img.height();
		while( h-- ) {
			float* ptr = map.ptr();
			for( size_t x = 0; x < img.width(); x++ )
				ptr[ x ] = ( float ) x / ( float ) ( img.width() - 1 );
			map++;
		}
	}

	Image out;

	img.colorCode( out, ICOLORCODE_PM3D, 0, 1.0f );
	out.save( "bla.png" );
}
