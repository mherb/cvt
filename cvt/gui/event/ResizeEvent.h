#ifndef CVTRESIZEEVENT_H
#define CVTRESIZEEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class ResizeEvent : public Event
	{
		friend class ApplicationX11;
		public:
			ResizeEvent( int width, int height, int oldwidth, int oldheight ) : Event( EVENT_RESIZE ), _width( width ), _height( height ), _oldwidth( oldwidth ), _oldheight( oldheight ) { };
			void getSize( int& width, int& height ) const { width = _width; height = _height;  };
			void getOldSize( int& width, int& height ) const { width = _oldwidth; height = _oldheight;  };
			int width() const { return _width; };
			int height() const { return _height; };

		private:
			int _width, _height;
			int _oldwidth, _oldheight;
	};
};

#endif
