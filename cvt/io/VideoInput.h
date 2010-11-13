#ifndef CVT_VIDEOINPUT_H
#define CVT_VIDEOINPUT_H

#include <cvt/gfx/Image.h>

namespace cvt {

	class VideoInput
	{
	public:
		virtual size_t width() = 0;
		virtual size_t height() = 0;
		virtual const Image & frame() const = 0;
		virtual void nextFrame() = 0;
		virtual const IOrder order() const = 0;
		virtual const IType type() const = 0;
		
	private:

	};
}

#endif