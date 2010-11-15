#ifndef CVT_EVENTTIMER_H
#define CVT_EVENTTIMER_H

namespace cvt {
	class EventTimer {
		public:
			EventTimer() {};
			virtual ~EventTimer() {};
			virtual void timeoutEvent() = 0;
	};
}

#endif
