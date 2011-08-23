#ifndef CVT_TIMERINFOLISTOSX_H
#define CVT_TIMERINFOLISTOSX_H

#include <cvt/gui/TimeoutHandler.h>

#include <stdint.h>
#include <list>
#include <utility>

namespace cvt {
	class TimerInfoOSX;

	class TimerInfoListOSX {
		public:
			TimerInfoListOSX();
			~TimerInfoListOSX();
			uint32_t registerTimer( size_t intervalms, TimeoutHandler* t );
			void unregisterTimer( uint32_t id );

		private:
			TimerInfoListOSX( const TimerInfoListOSX& tl );

			std::list<TimerInfoOSX*> _timers;
	};


}

#endif
