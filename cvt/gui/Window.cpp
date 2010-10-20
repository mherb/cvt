#include <cvt/gui/Window.h>
#include <cvt/gui/Application.h>

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
		gfx->getColor().set( 1.0f, 0.0f, 0.0f, 1.0f );
		gfx->fillRect( 0, 0, w, h );
		gfx->getColor().set( 0.0f, 1.0f, 0.0f, 1.0f );
		gfx->fillRect( 100, 100, 100, 100 );
//		gfx->drawText( 10, 10, "Test" );
	}

}
