#include <cvt/gui/Moveable.h>

namespace cvt {
	Moveable::Moveable( Widget* child ) : _child( child ), _activeWidget( NULL ), _activeMode( 0 )
	{
		if( _child )
			_child->setParent( this );
	}

	Moveable::~Moveable()
	{

	}

	void Moveable::paintEvent( PaintEvent* , GFX* gfx )
	{
		int w, h;

		size( w, h );
		gfx->color().set( 0.2f, 0.2f, 0.2f, 0.8f );
		gfx->fillRoundRect( 0, 0, w, h, 10.0f );

		Recti r;
		rect( r );
		paintChildren( gfx, r );
	}

	void Moveable::resizeEvent( ResizeEvent* )
	{
		resizeChildren();
	}

	void Moveable::mousePressEvent( MousePressEvent* event )
	{
		_activeWidget = childAt( event->x, event->y );
		if( _activeWidget ) {
			int cx, cy;
			_activeWidget->position( cx, cy );
			event->x -= cx;
			event->y -= cy;
			_activeWidget->mousePressEvent( event );
		} else {
			event->position( _lx, _ly );
			_activeMode = 1;
		}
	}

	void Moveable::mouseMoveEvent( MouseMoveEvent* event )
	{
		if( _activeWidget ) {
			int cx, cy;
			_activeWidget->position( cx, cy );
			event->x -= cx;
			event->y -= cy;
			_activeWidget->mouseMoveEvent( event );
		} else if( _activeMode == 1 ) {
			int cx, cy, nx, ny, dx, dy;
			position( cx, cy );
			event->position( nx, ny );
			dx = nx - _lx;
			dy = ny - _ly;
			setPosition( cx + dx, cy + dy );
			_lx = nx - dx;
			_ly = ny - dy;
			update();
		}
	}

	void Moveable::mouseReleaseEvent( MouseReleaseEvent* event )
	{
		if( _activeWidget ) {
			int cx, cy;
			_activeWidget->position( cx, cy );
			event->x -= cx;
			event->y -= cy;
			_activeWidget->mouseReleaseEvent( event );
		} else
			_activeMode = 1;
	}
}
