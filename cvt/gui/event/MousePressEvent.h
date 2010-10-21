#ifndef CVTMOUSEPRESSEVENT_H
#define CVTMOUSEPRESSEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class MousePressEvent : Event
	{
		public:
			MousePressEvent( int x, int y, int button ) : Event( EVENT_MOUSEPRESS ), _x( x ), _y( y ), _button( button ) { };
			void getPosition( int& x, int& y ) { x = _x; y = _y;  };
			unsigned int getButton() { return _button; };

		private:
			int _x, _y;
			unsigned int _button;
	};
};

#endif
