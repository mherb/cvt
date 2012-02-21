/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef GLIMAGE_H
#define GLIMAGE_H

#include "gui/GLObject.h"
#include "gfx/Image.h"

namespace cvt {
	class GLImage : public GLObject
	{
		public:
			GLImage();
			GLImage( const Image& i );
			virtual ~GLImage();
			virtual void draw() const;
			void setImage( const Image& i );

		private:
			GLuint _tex;
			float  _iw, _ih;
	};
}

#endif
