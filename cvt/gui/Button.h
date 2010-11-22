#ifndef CVT_BUTTON_H
#define CVT_BUTTON_H

#include <cvt/gui/Widget.h>
#include <cvt/util/Signal.h>

namespace cvt {
	class Button : public Widget
	{
		public:
			Button( const std::string& label );
			~Button();

			Signal<Button*> clicked;

		private:
			void paintEvent( PaintEvent* e, GFX* g );
			void mousePressEvent( MousePressEvent* event );
			void mouseReleaseEvent( MouseReleaseEvent* event );


			Button( const Button& b );

			bool _click;
			std::string _label;
	};
}

#endif
