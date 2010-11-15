#ifndef CVT_TIMERINFOLIST_H
#define CVT_TIMERINFOLIST_H

#include <cvt/util/Mutex.h>
#include <cvt/util/TQueue.h>
#include <cvt/gui/EventTimer.h>
#include <cvt/gui/event/TimerEvent.h>
#include <cvt/gui/internal/X.h>

#include <stdint.h>
#include <list>
#include <utility>

namespace cvt {
	class TimerInfo;

	class TimerInfoList {
		public:
			TimerInfoList();
			~TimerInfoList();
			uint32_t registerTimer( size_t intervalms, EventTimer* t );
			void unregisterTimer( uint32_t id );
			int nextTimeout();
			void queueEvents( TQueue<std::pair< ::Window,Event*> >* queue );

		private:
			void insertTimer( std::list<TimerInfo*>* list, TimerInfo* t );
			TimerInfoList( const TimerInfoList& tl );

			Mutex _mtx;
			std::list<TimerInfo*> _timers;
	};


}

#endif
