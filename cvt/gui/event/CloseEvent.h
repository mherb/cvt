#ifndef CVTCLOSEEVENT_H
#define CVTCLOSEEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class CloseEvent : public Event
	{
		public:
			CloseEvent() : Event( EVENT_CLOSE ) {};
	};
}

#endif
