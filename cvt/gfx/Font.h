/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_FONT_H
#define CVT_FONT_H

#include <cvt/geom/Rect.h>

namespace cvt {
	class Font {
		public:
			virtual ~Font() {};
			virtual int size() const = 0;
			virtual Recti stringBounds( const char* str, size_t n = 0 ) const = 0;
			virtual int advance( size_t glyph ) const = 0;
			virtual int ascent() const = 0;
			virtual int descent() const = 0;
	};
}

#endif
