#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/util/String.h>

#define BUTTERFLY 1

using namespace cvt;

class View : public Window
{
	public:
	View( ) : Window("GL-GFX")
	{
#ifndef BUTTERFLY
// Star
		_path.moveTo( 200.0f, 10.0f );
		_path.lineTo( 300.0f, 410.0f );
		_path.lineTo( 10.0f, 200.0f );
		_path.lineTo( 410.0f, 200.0f );
		_path.lineTo( 100.0f, 410.0f);
		_path.close();
#else
#include "butterfly.h"
		Matrix3f t;
		t.identity();
		t[ 0 ][ 0 ] = 3.0f;
		t[ 1 ][ 1 ] = 3.0f;
		p1.transform( t );
		p2.transform( t );
		p3.transform( t );
#endif
	}

	~View()
	{
	}

	void paintEvent( PaintEvent* e, GFX* g )
	{
		Window::paintEvent( e, g );
#ifndef BUTTERFLY
		g->color().set( 0.0f, 1.0f, 0.0f, 0.5f );
		g->fillPath( _path, GFX::WINDING_EVEN_ODD );
#else
		g->color().set( 0.95f, 0.5f, 0.0f, 1.0f );
		g->fillPath( p1 );
		g->color().set( 0.0f, 0.0f, 0.0f, 1.0f );
		g->fillPath( p3 );
		g->color().set( 1.0f, 0.95f, 0.9f, 1.0f );
		g->fillPath( p2 );

#endif
	}

	private:
#ifndef BUTTERFLY
		Pathf _path;
#else
		Pathf p1, p2, p3;
#endif
};

int main( int argc, char** argv )
{
	View ui;
	ui.setSize( 640, 480 );
	ui.show();
	Application::run();
	return 0;
}
