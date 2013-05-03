#ifndef CVT_X11KEYMAP_H
#define CVT_X11KEYMAP_H

#include <cvt/gui/Event.h>
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

	inline void X11KeyMap::mapToKeyEvent( KeyEvent& kev, XKeyEvent* xev )
	{
		char buf[ 10 ];
		KeySym keysym;

		int count = XLookupString( xev, buf, 10, &keysym, NULL );
		kev = KeyEvent( XKeySymToKeycode( keysym ), XModifierToKeyModifier( xev ), String( buf, count ) );
	}


	static inline KeyCode X11KeyMap::XKeySymToKeycode( KeySym sym )
	{

	}


	static KeyModifier XModifierToKeyModifier( XKeyEvent* xev )
	{
	
	}
}

#endif
