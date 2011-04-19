#include <cvt/gui/Label.h>

namespace cvt {
	Label::Label( const String& label, Alignment align  ) : _label( label ), _aligment( align )
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
		g->drawText( 0, 0, w, h, _aligment, _label.c_str() );
	}

}
