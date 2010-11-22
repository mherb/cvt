#ifndef CVTMOUSERELEASEEVENT_H
#define CVTMOUSERELEASEEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class MouseReleaseEvent : public Event
	{
		public:
			MouseReleaseEvent( int _x, int _y, int button ) : Event( EVENT_MOUSERELEASE ), x( _x ), y( _y ), _button( button ) { };
			void position( int& x, int& y ) { x = this->x; y = this->y;  };
			unsigned int button() { return _button; };

			int x, y;
		private:
			unsigned int _button;
	};
};

#endif
