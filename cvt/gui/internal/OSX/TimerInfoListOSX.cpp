/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/gui/internal/OSX/TimerInfoListOSX.h>
#include <cvt/gui/internal/OSX/TimerInfoOSX.h>

namespace cvt {
		TimerInfoListOSX::TimerInfoListOSX()
		{
		}

		TimerInfoListOSX::~TimerInfoListOSX()
		{
			// FIXME: do cleanup
		}


		uint32_t TimerInfoListOSX::registerTimer( size_t intervalms, TimeoutHandler* th )
		{
			TimerInfoOSX* ti = new TimerInfoOSX( intervalms, th );
			_timers.push_back( ti );
			return ti->id();
		}

		void TimerInfoListOSX::unregisterTimer( uint32_t id )
		{
			for( std::list<TimerInfoOSX*>::iterator it =_timers.begin() ; it != _timers.end(); ++it ) {
				if( ( *it )->id() == id  ) {
					TimerInfoOSX* ti = *it;
					_timers.erase( it );
					delete ti;
					return;
				}
			}
		}
}
