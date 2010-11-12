#ifndef CVTMOUSERELEASEEVENT_H
#define CVTMOUSERELEASEEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class MouseReleaseEvent : public Event
	{
		public:
			MouseReleaseEvent( int x, int y, int button ) : Event( EVENT_MOUSERELEASE ), _x( x ), _y( y ), _button( button ) { };
			void getPosition( int& x, int& y ) { x = _x; y = _y;  };
			unsigned int getButton() { return _button; };

		private:
			int _x, _y;
			unsigned int _button;
	};
};

#endif
