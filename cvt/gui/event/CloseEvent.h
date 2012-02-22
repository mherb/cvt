/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
