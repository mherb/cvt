#include <cvt/gfx/Image.h>
#include <cvt/gfx/GFX.h>
#include <cvt/gfx/GFXEngineImage.h>
#include <cvt/gfx/Clipping.h>

using namespace cvt;

int main()
{
	srand( time( NULL ) );
	Image out( 640, 480, IFormat::BGRA_UINT8 );
//	Image out( 640, 480, IFormat::BGRA_FLOAT );

	{
		GFXEngineImage ge( out );
		GFX g( &ge );
		g.color().set( 0.0f, 0.0f, 0.0f, 1.0f );
		g.fillRect( 0, 0, 640, 480 );
		g.color().set( 0.0f, 1.0f, 0.0f, 1.0f );
		g.fillRect( 100, 100, 200, 200 );
		g.color().set( 1.0f, 0.0f, 0.0f, 1.0f );

		Rectf r( 100, 100, 200, 200 );

		for( int i = 0; i < 5000; i++ ) {
			g.color().set( Math::rand( 0.0f, 1.0f ), Math::rand( 0.0f, 1.0f ), Math::rand( 0.0f, 1.0f ), 1.0f );
			Vector2f pt1, pt2;
			pt1.x = Math::rand( 0, 400 );
			pt1.y = Math::rand( 0, 400 );
			pt2.x = Math::rand( 0, 400 );
			pt2.y = Math::rand( 0, 400 );
			if( Clipping::clip( r, pt1, pt2 ) )
			g.drawLine( pt1, pt2 );
		}
	}
	out.save( "gfximg.png" );
}
