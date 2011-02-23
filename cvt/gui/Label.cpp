#include <cvt/gui/Label.h>

namespace cvt {
	Label::Label( const std::string& label ) : _label( label )
	{
	}

	Label::~Label()
	{

	}

	void Label::paintEvent( PaintEvent* event, GFX* g )
	{
		int w, h;
		g->color().set( 0.8f, 0.8f, 0.8f, 1.0f );
		size( w, h );
		g->drawText( 0, 0, w, h, _label.c_str() );
	}

}
