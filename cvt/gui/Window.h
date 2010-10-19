#ifndef CVTWINDOW_H
#define CVTWINDOW_H

#include <cvt/gui/Widget.h>

namespace cvt {

	class Window : public Widget
	{
		public:
			Window( const std::string& title );
			virtual ~Window();

			virtual void closeEvent( CloseEvent* event );
			virtual void paintEvent( PaintEvent* event, GFX* gfx );

		private:
			Window( const Window& w );
	};
}

#endif
