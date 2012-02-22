/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
