/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_DUMMYFONT_H
#define CVT_DUMMYFONT_H

#include <cvt/gfx/Font.h>

namespace cvt {
	class DummyFont : public Font
	{
		public:
			DummyFont() {};
        DummyFont( const DummyFont& ) : Font() {};
			int size() const { return 0; };
			Recti stringBounds( const char*, size_t = 0 ) const { return Recti(); };
			int advance( size_t ) const { return 0;}
			int ascent() const { return 0;};
			int descent() const { return 0;};
	};
}

#endif
