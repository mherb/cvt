#include <cvt/gfx/Image.h>
#include <cvt/gfx/GFX.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/io/ImageIO.h>

using namespace cvt;

int main()
{
	Image out( 640, 480, IFormat::BGRA_UINT8 );

	{
		GFXEngineImage ge( out );
		GFX g( &ge );
		g.color().set( 0.0f, 0.0f, 0.0f, 1.0f );
		g.fillRect( 0, 0, 640, 480 );
		g.color().set( 0.0f, 1.0f, 0.0f, 1.0f );
		g.fillRect( 100, 100, 200, 200 );
	}
	ImageIO::savePNG( out, "gfximg.png" );
}
