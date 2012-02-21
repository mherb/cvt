/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_VIDEOINPUT_H
#define CVT_VIDEOINPUT_H

#include <cvt/gfx/Image.h>

namespace cvt {

	class VideoInput
	{
	public:
		virtual ~VideoInput(){};
		virtual size_t width() const = 0;
		virtual size_t height() const = 0;
		virtual const Image & frame() const = 0;
		virtual void nextFrame() = 0;
		virtual const IFormat & format() const = 0;
	};
}

#endif
