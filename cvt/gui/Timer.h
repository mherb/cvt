#ifndef CVT_TIMER_H
#define CVT_TIMER_H

namespace cvt {
	class TimerInfo;

	class Timer {
		public:
			Timer();
			Timer( const Timer& t );
			~Timer();
			start();
			stop();
			virtual void onTimeout() {};
		private:
			size_t _interval
			TimerInfo* _ti;
	};

}
#endif
