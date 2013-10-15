#ifndef CVTMOUSEMOVEEVENT_H
#define CVTMOUSEMOVEEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class MouseMoveEvent : public Event
	{
		public:
			MouseMoveEvent( int _x, int _y, unsigned int buttonMask ) : Event( EVENT_MOUSEMOVE ), x( _x ), y( _y ), _buttonMask( buttonMask ) { };
			void position( int& x, int& y ) { x = this->x; y = this->y;  };
			unsigned int buttonMask() { return _buttonMask; };

			int x, y;
		private:
			unsigned int _buttonMask;
	};
};

#endif
