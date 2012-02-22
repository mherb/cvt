/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVTMOVEEVENT_H
#define CVTMOVEEVENT_H

#include <cvt/gui/event/Event.h>

namespace cvt {
	class MoveEvent : public Event
	{
		friend class ApplicationX11;
		public:
			MoveEvent( int _x, int _y, int _oldx, int _oldy ) : Event( EVENT_MOVE ), x( _x ), y( _y ), oldx( _oldx ), oldy( _oldy ) { };
			void position( int& _x, int& _y ) const { _x = x; _y = y; };
			void oldPosition( int& _x, int& _y ) const { _x = oldx; _y = oldy;  };

			int x, y;
			int oldx, oldy;
	};
};

#endif
