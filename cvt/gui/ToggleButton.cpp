#include <cvt/gui/ToggleButton.h>

namespace cvt {
	ToggleButton::ToggleButton( bool state, GFX::Icon off, GFX::Icon on ) : _state( state ), _iconon( on ), _iconoff( off )
	{
	}

	ToggleButton::~ToggleButton()
	{

	}

	void ToggleButton::paintEvent( PaintEvent* event, GFX* g )
	{
		int w, h;
		size( w, h );
		w = ( w - 16 ) >> 1;
		h = ( h - 16 ) >> 1;
		g->color().set( 0.6f, 0.6f, 0.6f, 1.0f );
		g->drawIcon( w, h, _state? _iconon : _iconoff );
	}

	void ToggleButton::mouseReleaseEvent( MouseReleaseEvent* event )
	{
		Recti self;
		size( self.width, self.height );
		if( self.contains( event->x, event->y ) && event->button() == 1 ) {
			_state = !_state;
			toggled.notify( this );
		}
		update();
	}

}
