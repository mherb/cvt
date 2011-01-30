#include <cvt/gui/Window.h>
#include <cvt/gui/Application.h>

#include <cstdio>

namespace cvt {
	Window::Window( const std::string& title ) : WidgetContainer( true )
	{
		setTitle( title );
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
	}

	void Window::paintEvent( PaintEvent* , GFX* gfx )
	{
		Recti r;

		rect( r );
		gfx->color().set( 0.4f, 0.4f, 0.4f, 1.0f );
		gfx->fillRect( 0, 0, r.width, r.height );
		paintChildren( gfx, r );
	}

	GFXEngine* Window::gfxEngine()
	{
		return Widget::gfxEngine();
	}
}
