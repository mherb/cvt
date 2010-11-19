#ifndef CVT_VIDEOINPUT_H
#define CVT_VIDEOINPUT_H

#include <cvt/gfx/Image.h>

namespace cvt {

	class VideoInput
	{
	public:
		virtual size_t width() const = 0;
		virtual size_t height() const = 0;
		virtual const Image & frame() const = 0;
		virtual void nextFrame() = 0;
		virtual const IFormat & format() const = 0;
	};
}

#endif
