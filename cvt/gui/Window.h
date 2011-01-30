#ifndef CVTWINDOW_H
#define CVTWINDOW_H

#include <cvt/gfx/Drawable.h>
#include <cvt/gui/WidgetContainer.h>

namespace cvt {

	class Window : public WidgetContainer, public Drawable
	{
		public:
			Window( const std::string& title );
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
