#include <cvt/gui/GFX.h>

namespace cvt {
	GFX::GFX()
	{
		setDefault();
	}

	GFX::~GFX()
	{
	}

	void GFX::setDefault()
	{
		_color.set( 0.0f, 0.0f, 0.0f, 1.0f );
	}
}

