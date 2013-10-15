#ifndef CVT_TIMERINFOLIST_H
#define CVT_TIMERINFOLIST_H

#include <cvt/gui/TimeoutHandler.h>

#include <stdint.h>
#include <iostream>
#include <list>
#include <utility>

namespace cvt {
	class TimerInfo;

	class TimerInfoList {
		public:
			TimerInfoList();
			~TimerInfoList();
			void handleTimers();
			uint32_t registerTimer( size_t intervalms, TimeoutHandler* t );
			void unregisterTimer( uint32_t id );
			int nextTimeout();

		private:
			void insertTimer( std::list<TimerInfo*>* list, TimerInfo* t );
			TimerInfoList( const TimerInfoList& tl );

			std::list<TimerInfo*> _timers;
	};


}

#endif
