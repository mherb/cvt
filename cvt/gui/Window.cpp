#include <cvt/gui/Window.h>
#include <cvt/gui/Application.h>

#include <cvt/io/ImageIO.h>
#include <cvt/io/Resources.h>

#include <cstdio>

namespace cvt {
	Window::Window( const std::string& title ) : WidgetContainer( true ), mouse( false )
	{
		setTitle( title );

		cvt::Resources resources;
		ImageIO::loadPNG( img, resources.find( "lena.png" ) );

		return;
	}

	Window::~Window( )
	{
	}

	void Window::closeEvent( CloseEvent* )
	{
		Application::exit();
	}


	void Window::moveEvent( MoveEvent* )
	{
		update();
	}

	void Window::paintEvent( PaintEvent* , GFX* gfx )
	{
		int w, h, x, y;
		size( w, h );


		gfx->color().set( 0.4f, 0.4f, 0.4f, 1.0f );
		gfx->fillRect( 0, 0, w, h );

		gfx->color().set( 0.4f, 0.4f, 0.4f, 1.0f );
		gfx->drawImage( 100, 100, 100, 100, img );

		gfx->color().set( 0.8f, 0.8f, 0.8f, 1.0f );
		gfx->drawIcon( 100, 100, GFX::ICON_CIRCLE );
		gfx->drawIcon( 100, 116, GFX::ICON_CIRCLEUP );
		gfx->drawIcon( 100, 132, GFX::ICON_CIRCLEDOWN );


		gfx->color().set( 0.0f, 0.0f, 0.0f, 1.0f );
		char buf[ 200 ];

		sprintf( buf, "Size: %d x %d", w, h );
		gfx->drawText( 10, 20, buf );

		position( x, y );
		sprintf( buf, "Position: %d x %d", x, y );
		gfx->drawText( 10, 32, buf );

		Recti r;
		rect( r );
		paintChildren( gfx, r );
	}

}
