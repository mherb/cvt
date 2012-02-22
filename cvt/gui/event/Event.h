/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVTEVENT_H
#define CVTEVENT_H

namespace cvt {

	enum EventType {
		EVENT_NONE = 0,
		EVENT_RESIZE,
		EVENT_MOVE,
		EVENT_SHOW,
		EVENT_HIDE,
		EVENT_CLOSE,
		EVENT_PAINT,
		EVENT_MOUSEENTER,
		EVENT_MOUSELEAVE,
		EVENT_MOUSEMOVE,
		EVENT_MOUSEPRESS,
		EVENT_MOUSERELEASE,
		EVENT_MOUSEWHEEL,
		EVENT_KEYPRESS,
		EVENT_KEYRELEASE,
		EVENT_TIMER
	};

	class Event {
		public:
			Event( EventType t ) : _type( t ) {};
			virtual ~Event() {};
			EventType type() const { return _type; };

		private:
			EventType _type;
	};
}

#endif
