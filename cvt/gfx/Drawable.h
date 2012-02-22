/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_DRAWABLE_H
#define CVT_DRAWABLE_H

namespace cvt {
	class GFXEngine;

	class Drawable
	{
		public:
			virtual ~Drawable() {};
			virtual GFXEngine* gfxEngine() = 0;
	};
}

#endif
