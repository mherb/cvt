#include <cvt/io/Postscript.h>
#include <cvt/gfx/GFX.h>

using namespace cvt;

int main()
{
	Postscript ps( "out.ps" );
	GFX g( &ps );
	g.color().set( 1.0, 0.0f, 0.0f, 1.0f );
	g.fillRect( 0, 0, 100, 100 );
	g.setLineWidth( 2 );
	g.color().set( 0.0, 1.0f, 0.0f, 1.0f );
	g.drawLine( 10, 10, 100, 100 );
}
