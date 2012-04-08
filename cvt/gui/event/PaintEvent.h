/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVTPAINTEVENT_H
#define CVTPAINTEVENT_H

#include <cvt/gui/event/Event.h>
#include <cvt/geom/Rect.h>

namespace cvt {
	class PaintEvent : public Event
	{
		public:
			PaintEvent( int x, int y, int width, int height ) : Event( EVENT_PAINT ), rect( x, y, width, height ) {};
			PaintEvent( const Recti& r ) : Event( EVENT_PAINT ), rect( r ) {};
			void getSize( int& width, int& height ) const { width = rect.width; height = rect.height;  };
			void getPosition( int& x, int& y ) const { x = rect.x; y = rect.y; };
			void getRect( Recti& r ) const { r = rect; };

		private:
			Recti rect;
	};
};

#endif
