#ifndef CVT_TIMEOUTHANDLER_H
#define CVT_TIMEOUTHANDLER_H

namespace cvt {
	class TimeoutHandler {
		public:
			TimeoutHandler() {};
			virtual ~TimeoutHandler() {};
			virtual void onTimeout() = 0;
	};
}

#endif
