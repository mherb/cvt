#ifndef CVT_X11KEYMAP_H
#define CVT_X11KEYMAP_H

#include <cvt/gui/Events.h>
#include <cvt/gui/internal/X11/X.h>

namespace cvt {

	class X11KeyMap
	{
		public:
			static void mapToKeyEvent( KeyEvent& kev, XKeyEvent* xev );
		private:
			static KeyCode XKeySymToKeycode( KeySym sym );
			static KeyModifier XModifierToKeyModifier( XKeyEvent* xev );

			X11KeyMap();
			~X11KeyMap();
			X11KeyMap( const X11KeyMap& );
	};

}

#endif
