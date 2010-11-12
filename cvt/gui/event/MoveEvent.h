#ifndef CVTMOVEEVENT_H
#define CVTMOVEEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class MoveEvent : public Event
	{
		friend class ApplicationX11;
		public:
			MoveEvent( int x, int y, int oldx, int oldy ) : Event( EVENT_MOVE ), _x( x ), _y( y ), _oldx( oldx ), _oldy( oldy ) { };
			void getPosition( int& x, int& y ) const { x = _x; y = _y;  };
			void getOldPosition( int& x, int& y ) const { x = _oldx; y = _oldy;  };
			int x() const { return _x; };
			int y() const { return _y; };

		private:
			int _x, _y;
			int _oldx, _oldy;
	};
};

#endif
