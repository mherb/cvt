#ifndef CVT_TIMEREVENT_H
#define CVT_TIMEREVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class TimerInfo;
	class TimerEvent : public Event
	{
		public:
			TimerEvent( TimerInfo* t ) : Event( EVENT_TIMER ), timerinfo( t ) {};
			TimerInfo* timerinfo;
	};
}

#endif
