#ifndef CVTMOUSEMOVEEVENT_H
#define CVTMOUSEMOVEEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class MouseMoveEvent : public Event
	{
		public:
			MouseMoveEvent( int _x, int _y ) : Event( EVENT_MOUSEMOVE ), x( _x ), y( _y ) { };
			void position( int& x, int& y ) { x = this->x; y = this->y;  };

			int x, y;
	};
};

#endif
