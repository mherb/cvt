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
		EVENT_KEYRELEASE
	};

	class Event {
		public:
			Event( EventType t ) : _type( t ) {};
			~Event() {};
			EventType type() const { return _type; };

		private:
			EventType _type;
	};
}

#endif
