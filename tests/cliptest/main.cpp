#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>
#include <cvt/gfx/Clipping.h>

using namespace cvt;

class ClipUI : public Window
{
	public:
	ClipUI() : Window("SplineUI")
	{
		_selection = 0;
		_rect.set( 100, 100, 300, 300 );
	}

	void paintEvent( PaintEvent* e, GFX* g )
	{
		Window::paintEvent( e, g );
		g->color().set( 1.0f, 1.0f, 1.0f, 1.0f );
		g->fillRect( _rect );

		if( _selection == 1 ) {
			g->color().set( 0.0f, 1.0f, 0.0f, 1.0f );
			g->setLineWidth( 1.0f );
			g->drawLine( _out[ 0 ].x, _out[ 0 ].y , _out[ 1 ].x, _out[ 1 ].y );
		}

	}

	void mousePressEvent( MousePressEvent* event )
	{
		_pt[ 0 ].set( event->x, event->y );
	}

	void mouseMoveEvent( MouseMoveEvent* event )
	{
		_pt[ 1 ].set( event->x, event->y );
		if( _pt[ 0 ].x != _pt[ 1 ].x || _pt[ 0 ].y != _pt[ 1 ].y ) {
			_selection = 1;
			_out[ 0 ] = _pt[ 0 ];
			_out[ 1 ] = _pt[ 1 ];
			if( !Clipping::clip( _rect, _out[ 0 ], _out[ 1 ] ) )
				_selection = 0;
			update();
		}
	}

	void mouseReleaseEvent( MouseReleaseEvent* e )
	{
	}

	private:
		int _selection;
		Recti _rect;
		Vector2i _pt[ 2 ];
		Vector2i _out[ 2 ];
};

int main()
{
	ClipUI ui;
	ui.setSize( 640, 480 );
	ui.show();
	Application::run();
	return 0;
}
