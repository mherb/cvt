#ifndef CVTRESIZEEVENT_H
#define CVTRESIZEEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class ResizeEvent : Event
	{
		public:
			ResizeEvent( int width, int height, int oldwidth, int oldheight ) : Event( EVENT_RESIZE ), _width( width ), _height( height ), _oldwidth( oldwidth ), _oldheight( oldheight ) { };
			void getSize( int& width, int& height ) { width = _width; height = _height;  };
			void getOldSize( int& width, int& height ) { width = _oldwidth; height = _oldheight;  };

		private:
			int _width, _height;
			int _oldwidth, _oldheight;
	};
};

#endif
