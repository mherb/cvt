/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVTWINDOW_H
#define CVTWINDOW_H

#include <cvt/gfx/Drawable.h>
#include <cvt/gui/WidgetContainer.h>
#include <cvt/util/String.h>

namespace cvt {

	class Window : public WidgetContainer, public Drawable
	{
		public:
			Window( const String& title );
			virtual ~Window();

			virtual void closeEvent( CloseEvent* event );
			virtual void moveEvent( MoveEvent* event );
			virtual void paintEvent( PaintEvent* event, GFX* gfx );
			GFXEngine* gfxEngine();

		private:
			Window( const Window& w );
	};
}

#endif
