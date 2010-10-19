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
		Recti r;
		getRect( r );
//		gfx->getColor().set( 0.5f, 0.5f, 0.5f, 1.0f );
		gfx->fillRect( r );
	}

}
