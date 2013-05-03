/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVTMOUSEPRESSEVENT_H
#define CVTMOUSEPRESSEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class MousePressEvent : public Event
	{
		public:
			MousePressEvent( int _x, int _y, int button ) : Event( EVENT_MOUSEPRESS ), x( _x ), y( _y ), _button( button ) { }
			void position( int& x, int& y ) { x = this->x; y = this->y;  }
			unsigned int button() { return _button; };

			int x, y;
		private:
			unsigned int _button;
	};
};

#endif
