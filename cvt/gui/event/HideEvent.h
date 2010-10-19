#ifndef CVTHHIDEEVENT_H
#define CVTHHIDEEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class HideEvent : public Event
	{
		public:
			HideEvent() : Event( EVENT_HIDE ) {};
	};
}

#endif
