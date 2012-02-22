/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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

