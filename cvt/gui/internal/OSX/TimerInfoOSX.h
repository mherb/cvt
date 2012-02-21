/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_TIMERINFOOSX_H
#define CVT_TIMERINFOOSX_H

#include <cvt/util/Time.h>
#include <cvt/gui/TimeoutHandler.h>
#include <cvt/util/IDGenerator.h>

namespace cvt {

	class TimerInfoOSX {
		friend class TimerInfoListOSX;

		public:
			TimerInfoOSX( size_t msinterval, TimeoutHandler* handler );
			TimerInfoOSX( const TimerInfoOSX& ti );
			uint32_t id() const;

		private:
			~TimerInfoOSX();

			uint32_t _id;
			void* _nstimer;
			void* _nstimerhandler;

			static IDGenerator _idgen;
	};

	inline uint32_t TimerInfoOSX::id() const
	{
		return _id;
	}
}

#endif
