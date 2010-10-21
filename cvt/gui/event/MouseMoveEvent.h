#ifndef CVTMOUSEMOVEEVENT_H
#define CVTMOUSEMOVEEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class MouseMoveEvent : Event
	{
		public:
			MouseMoveEvent( int x, int y ) : Event( EVENT_MOUSEMOVE ), _x( x ), _y( y ) { };
			void getPosition( int& x, int& y ) { x = _x; y = _y;  };

		private:
			int _x, _y;
	};
};

#endif
