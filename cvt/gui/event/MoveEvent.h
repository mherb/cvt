#ifndef CVTMOVEEVENT_H
#define CVTMOVEEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class MoveEvent : Event
	{
		public:
			MoveEvent( int x, int y, int oldx, int oldy ) : Event( EVENT_MOVE ), _x( x ), _y( y ), _oldx( oldx ), _oldy( oldy ) { };
			void getPosition( int& x, int& y ) { x = _x; y = _y;  };
			void getOldPosition( int& x, int& y ) { x = _oldx; y = _oldy;  };

		private:
			int _x, _y;
			int _oldx, _oldy;
	};
};

#endif
