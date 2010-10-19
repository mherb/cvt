#ifndef CVTSHOWEVENT_H
#define CVTSHOWEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class ShowEvent : public Event
	{
		public:
			ShowEvent() : Event( EVENT_SHOW ) {};
	};
}

#endif
