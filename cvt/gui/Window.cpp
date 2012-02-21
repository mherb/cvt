/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/gui/Window.h>
#include <cvt/gui/Application.h>

#include <cstdio>

namespace cvt {
	Window::Window( const String& title ) : WidgetContainer( true )
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
		Recti r = rect();
		gfx->color().set( 0.4f, 0.4f, 0.4f, 1.0f );
		gfx->fillRect( 0, 0, r.width, r.height );
		gfx->color().set( 0.0f, 0.0f, 1.0f, 1.0f );
		int w, h;
		size( w, h );
		paintChildren( gfx, r );
	}

	GFXEngine* Window::gfxEngine()
	{
		return Widget::gfxEngine();
	}
}
