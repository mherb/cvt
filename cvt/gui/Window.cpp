#include <cvt/gui/Window.h>
#include <cvt/gui/Application.h>

#include <cstdio>

namespace cvt {
	Window::Window( const std::string& title ) : Widget( NULL )
	{
		setTitle( title );
	}

	Window::~Window( )
	{
	}

	void Window::closeEvent( CloseEvent* event )
	{
		Application::exit();
	}

	void Window::paintEvent( PaintEvent* event, GFX* gfx )
	{
		int w, h;
		getSize( w, h );
		gfx->getColor().set( 0.3f, 0.3f, 0.3f, 1.0f );
		gfx->fillRect( 0, 0, w, h );
//		gfx->getColor().set( 0.0f, 1.0f, 0.0f, 1.0f );
//		gfx->fillRect( 100, 100, 100, 100 );


		gfx->getColor().set( 0.0f, 0.0f, 0.0f, 1.0f );
		char buf[ 20 ];
		sprintf( buf, "Size: %d x %d", w, h );
		gfx->drawText( 10, 20, buf );

//		gfx->getColor().set( 0.0f, 1.0f, 1.0f, 1.0f );
//		gfx->fillRect( 10, 20, 1, 1 );

	}

}
