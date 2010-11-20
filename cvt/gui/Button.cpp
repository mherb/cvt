#include <cvt/gui/Button.h>

namespace cvt {
	Button::Button( const std::string& label ) : _label( label )
	{
	}

	Button::~Button()
	{

	}

	void Button::paintEvent( PaintEvent* event, GFX* g )
	{
		int w, h;
		g->color().set( 1.0f, 0.0f, 0.0f, 1.0f );
		size( w, h );
		g->fillRect( 0, 0, w, h );
	}

}
