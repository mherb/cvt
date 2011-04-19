#ifndef CVT_TOGGLEBUTTON_H
#define CVT_TOGGLEBUTTON_H

#include <cvt/gui/Widget.h>
#include <cvt/util/Signal.h>
#include <cvt/util/String.h>

namespace cvt {
	class ToggleButton : public Widget
	{
		public:
			ToggleButton( const String& label, bool state = false, GFX::Icon off = GFX::ICON_RECT, GFX::Icon on = GFX::ICON_RECTCROSS );
			ToggleButton( bool state = false, GFX::Icon off = GFX::ICON_RECT, GFX::Icon on = GFX::ICON_RECTCROSS );
			~ToggleButton();
			bool state() const;

			Signal<ToggleButton*> toggled;

		private:
			void paintEvent( PaintEvent* e, GFX* g );
			void mouseReleaseEvent( MouseReleaseEvent* event );

			ToggleButton( const ToggleButton& b );

			String _label;
			bool _state;
			GFX::Icon _iconon;
			GFX::Icon _iconoff;
	};

	inline bool ToggleButton::state() const
	{
		return _state;
	}
}

#endif
