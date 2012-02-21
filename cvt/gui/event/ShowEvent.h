/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
