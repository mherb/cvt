#ifndef CVT_MOVEABLE_H
#define CVT_MOVEABLE_H

#include <cvt/gui/WidgetContainer.h>

namespace cvt {
	class Moveable : public WidgetContainer
	{
		public:
			Moveable( Widget* child );
			~Moveable();

			Widget* child();

		private:
			void paintEvent( PaintEvent* ev, GFX* g );
			void resizeEvent( ResizeEvent* ev );
			void mousePressEvent( MousePressEvent* ev );
			void mouseMoveEvent( MouseMoveEvent* ev );
			void mouseReleaseEvent( MouseReleaseEvent* ev );

			Widget* _child;
			Widget* _activeWidget;
			int _activeMode;
			int _lx, _ly;
	};

	inline Widget* Moveable::child()
	{
		return _child;
	}
}

#endif
