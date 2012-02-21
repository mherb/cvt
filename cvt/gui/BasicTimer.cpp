/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/gui/BasicTimer.h>
#include <cvt/gui/Application.h>

namespace cvt {
	void BasicTimer::start()
	{
		_id = Application::registerTimer( _interval, this );
	}

	void BasicTimer::stop()
	{
		if( _id ) {
			Application::unregisterTimer( _id );
			_id = 0;
		}
	}
}
