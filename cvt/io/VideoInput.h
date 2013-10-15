#ifndef CVT_VIDEOINPUT_H
#define CVT_VIDEOINPUT_H

#include <cvt/gfx/Image.h>

namespace cvt {

	class VideoInput
	{
	public:
		virtual ~VideoInput() {}
		virtual size_t width() const = 0;
		virtual size_t height() const = 0;

		virtual const Image & frame() const = 0;

		/* trigger / load next frame, timeOut in ms */
		virtual bool nextFrame( size_t timeOut = 5 ) = 0;
		
		virtual const IFormat & format() const = 0;
	};
}

#endif
