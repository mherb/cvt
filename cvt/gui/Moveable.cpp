#include <cvt/gui/Moveable.h>

namespace cvt {
	Moveable::Moveable( Widget* child ) : _child( child ),
										  _activeWidget( NULL ),
										  _activeMode( 0 ),
										  _title("Unnamed"),
										  _togglebutton( false, GFX::ICON_CIRCLEUP, GFX::ICON_CIRCLEDOWN ),
										  _onToggleDelegate( this, &Moveable::onToggle)
	{

		WidgetLayout layout;

		if( _child ) {
			layout.setAnchoredLeftRight( 8, 8 );
			layout.setAnchoredTopBottom( 20, 8 );
			addWidget( _child, layout );
		}
		layout.setAnchoredRight( 2, 16 );
		layout.setAnchoredTop( 2, 16 );
		_togglebutton.toggled.add( &_onToggleDelegate );
		addWidget( &_togglebutton, layout );
	}

	Moveable::~Moveable()
	{

	}

	void Moveable::setTitle( const std::string& title )
	{
		_title = title;
	}
	void Moveable::paintEvent( PaintEvent* , GFX* gfx )
	{
		int w, h;

		size( w, h );
		gfx->color().set( 0.2f, 0.2f, 0.2f, 0.8f );
		gfx->fillRoundRect( 0, 0, w, h, 10.0f );
		gfx->color().set( 0.6f, 0.6f, 0.6f, 1.0f );
		gfx->drawText( 8, 15, _title.c_str() );


		Recti r;
		rect( r );
		paintChildren( gfx, r );

		if( !_togglebutton.state() ) {
			gfx->color().set( 0.6f, 0.6f, 0.6f, 1.0f );
			gfx->drawIcon( w - 18, h - 18, GFX::ICON_CORNER );
		}
	}

	void Moveable::resizeEvent( ResizeEvent* )
	{
		resizeChildren();
	}


	void Moveable::onToggle( ToggleButton* button )
	{
		int w, h;
		size( w, h );

		if( button->state() ) {
			if( _child )
				_child->setVisible( false );
			_oldheight = h;
			setSize( w, 20 );
		} else {
			if( _child )
				_child->setVisible( true );
			setSize( w, _oldheight );
		}
		update();
	}

	void Moveable::mousePressEvent( MousePressEvent* event )
	{
		int gx, gy;
		Recti rcorner;


		event->position( _lx, _ly );
		size( rcorner.x, rcorner.y );
		rcorner.x -= 20;
		rcorner.y -= 20;
		rcorner.setSize( 16, 16 );
		if( rcorner.contains( _lx, _ly  ) && !_togglebutton.state() ) {
			_activeMode = 2;
		} else {
			event->position( gx, gy );
			mapGlobal( gx, gy );
			_activeWidget = childAt( gx, gy );
			if( _activeWidget ) {
				mapGlobal( event->x, event->y );
				_activeWidget->mapLocal( event->x, event->y );
				_activeWidget->mousePressEvent( event );
			} else {
				_activeMode = 1;
			}
		}
	}

	void Moveable::mouseMoveEvent( MouseMoveEvent* event )
	{
		if( _activeMode == 1 ) {
			int cx, cy, nx, ny, dx, dy;
			position( cx, cy );
			event->position( nx, ny );
			dx = nx - _lx;
			dy = ny - _ly;
			setPosition( cx + dx, cy + dy );
			_lx = nx - dx;
			_ly = ny - dy;
			update();
		} else if( _activeMode == 2 ) {
			int cw, ch, nx, ny, dx, dy;
			event->position( nx, ny );
			dx = nx - _lx;
			dy = ny - _ly;
			size( cw, ch );
			cw = Math::max( cw + dx, 32 );
			ch = Math::max( ch + dy, 32 );
			setSize( cw, ch );
			_lx = nx;
			_ly = ny;
			update();
		} else if( _activeWidget ) {
			mapGlobal( event->x, event->y );
			_activeWidget->mapLocal( event->x, event->y );
			_activeWidget->mouseMoveEvent( event );
		}
	}

	void Moveable::mouseReleaseEvent( MouseReleaseEvent* event )
	{
		if( _activeWidget ) {
			mapGlobal( event->x, event->y );
			_activeWidget->mapLocal( event->x, event->y );
			_activeWidget->mouseReleaseEvent( event );
		} else
			_activeMode = 0;
	}
}
