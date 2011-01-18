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
		int tw = g->textWidth( _label.c_str() );
		g->drawText( ( ( float ) w - ( float ) tw ) * 0.5f , ( ( float ) h + 11.0f ) * 0.5f, _label.c_str() );
	}

}
