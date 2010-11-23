#include <cvt/gui/Moveable.h>

namespace cvt {
	Moveable::Moveable( Widget* child ) : _child( child ), _active( false )
	{
		if( _child )
			_child->setParent( this );
	}

	Moveable::~Moveable()
	{

	}

	void Moveable::paintEvent( PaintEvent* ev, GFX* gfx )
	{
		int w, h;

		size( w, h );
		gfx->color().set( 0.2f, 0.2f, 0.2f, 0.8f );
		gfx->fillRoundRect( 0, 0, w, h, 10.0f );
	}

	void Moveable::resizeEvent( ResizeEvent* ev )
	{

	}

	void Moveable::mousePressEvent( MousePressEvent* ev )
	{
		_active = true;
		ev->position( _lx, _ly );
	}

	void Moveable::mouseMoveEvent( MouseMoveEvent* ev )
	{
		int cx, cy, nx, ny, dx, dy;
		position( cx, cy );
		ev->position( nx, ny );
		dx = nx - _lx;
		dy = ny - _ly;
		setPosition( cx + dx, cy + dy );
		_lx = nx - dx;
		_ly = ny - dy;
		update();
	}

	void Moveable::mouseReleaseEvent( MouseReleaseEvent* ev )
	{
		_active = false;
	}

}
