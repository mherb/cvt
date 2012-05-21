/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_BASICTIMER_H
#define CVT_BASICTIMER_H

#include <cvt/gui/TimeoutHandler.h>
#include <cvt/util/Signal.h>
#include <iostream>
#include <stdint.h>

namespace cvt {
	class BasicTimer : public TimeoutHandler {
		public:
			BasicTimer( size_t interval_ms );
			~BasicTimer();
			void start();
			void stop();
			Signal<BasicTimer*> timeout;
		private:
			void onTimeout();

			uint32_t _id;
			size_t _interval;
	};

	inline BasicTimer::BasicTimer( size_t ms ) : _id( 0 ), _interval( ms )
	{
	}

	inline BasicTimer::~BasicTimer( )
	{
		stop();
	}

	inline void BasicTimer::onTimeout()
	{
		timeout.notify( this );
	}
};


#endif
