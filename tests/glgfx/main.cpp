#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/util/String.h>
#include <cvt/util/Time.h>
#include <cvt/gui/BasicTimer.h>

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
		t.setIdentity();
		t[ 0 ][ 0 ] = 2.0f;
		t[ 1 ][ 1 ] = 2.0f;
		p1.transform( t );
		p2.transform( t );
		p3.transform( t );
#endif
		_t.reset();
		}

	~View()
	{
	}

	void timeout( BasicTimer* )
	{
		Matrix3f t1, t2;
		t1.setIdentity();
		t1[ 0 ][ 2 ] = 400.0f;
		t1[ 1 ][ 2 ] = 400.0f;
		t2.setRotationZ( Math::deg2Rad( 1.0f ) );
		t1 = t1 * t2;
		t2.setIdentity();
		t2[ 0 ][ 2 ] = -400.0f;
		t2[ 1 ][ 2 ] = -400.0f;
		t1 = t1 * t2;
		p1.transform( t1 );
		p2.transform( t1 );
		p3.transform( t1 );

		update();
	}

	void paintEvent( PaintEvent* e, GFX* g )
	{
		static size_t frames = 0;
		Window::paintEvent( e, g );
#ifndef BUTTERFLY
		g->color().set( 0.0f, 1.0f, 0.0f, 0.5f );
		g->fillPath( _path, GFX::WINDING_EVEN_ODD );
#else
		g->setLineWidth( 0.0f );
		g->color().set( 0.95f, 0.5f, 0.0f, 1.0f );
		g->strokePath( p1 );
		g->color().set( 0.0f, 0.0f, 0.0f, 1.0f );
		g->strokePath( p3 );
		g->color().set( 1.0f, 0.95f, 0.9f, 1.0f );
		g->strokePath( p2 );

		frames++;

		if( _t.elapsedSeconds() > 5.0f )
		{
			std::cout << "FPS: " << frames / _t.elapsedSeconds() << std::endl;
			frames = 0;
			_t.reset();
		}

#endif
	}

	private:
#ifndef BUTTERFLY
		Pathf _path;
#else
		Pathf p1, p2, p3;
#endif
		Time _t;
};

int main( int argc, char** argv )
{
	View ui;
	Delegate<void ( BasicTimer* )> d( &ui, &View::timeout );
	BasicTimer timer( 33 );
	timer.timeout.add( d );
	timer.start();

	ui.setSize( 640, 480 );
	ui.show();
	Application::run();
	return 0;
}
