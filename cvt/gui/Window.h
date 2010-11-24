#ifndef CVTWINDOW_H
#define CVTWINDOW_H

#include <cvt/gui/WidgetContainer.h>

namespace cvt {

	class Window : public WidgetContainer
	{
		public:
			Window( const std::string& title );
			virtual ~Window();

			virtual void closeEvent( CloseEvent* event );
			virtual void moveEvent( MoveEvent* event );
			virtual void paintEvent( PaintEvent* event, GFX* gfx );

		private:
			Window( const Window& w );
	};
}

#endif
