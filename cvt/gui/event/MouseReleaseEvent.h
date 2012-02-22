/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVTMOUSERELEASEEVENT_H
#define CVTMOUSERELEASEEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class MouseReleaseEvent : public Event
	{
		public:
			MouseReleaseEvent( int _x, int _y, int button ) : Event( EVENT_MOUSERELEASE ), x( _x ), y( _y ), _button( button ) { };
			void position( int& x, int& y ) { x = this->x; y = this->y;  };
			unsigned int button() { return _button; };

			int x, y;
		private:
			unsigned int _button;
	};
};

#endif
