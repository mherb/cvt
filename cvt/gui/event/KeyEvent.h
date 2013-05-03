/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_KEYEVENT_H
#define CVT_KEYEVENT_H

#include <cvt/gui/event/Event.h>
#include <cvt/util/Flags.h>
#include <cvt/gui/KeyCodes.h>

namespace cvt {

	enum KeyModifierFlags {
		NO_MODIFIER = ( 0 << 0 ),
		SHIFT_MODIFIER = ( 1 << 0 ),
		CTRL_MODIFIER  = ( 1 << 1 ),
		ALT_MODIFIER   = ( 1 << 2 ),
		META_MODIFIER  = ( 1 << 3 ),
		KP_MODIFIER    = ( 1 << 4 )
	};

	CVT_ENUM_TO_FLAGS( KeyModifierFlags, KeyModifier)

	class KeyEvent : public Event
	{
		public:
							 KeyEvent( KeyCode keycode = KEY_UNKNOWN, KeyModifier modifier = NO_MODIFIER, const String& text = "" ) : Event( EVENT_KEY ), _key( keycode ), _modifier( modifier ), _text( text ) {}
							 KeyEvent( const KeyEvent& kev ) : Event( EVENT_KEY ), _key( kev._key ), _modifier( kev._modifier), _text( kev._text ) {}

			int				 key();
			KeyModifier		 modifiers() const { return _modifier; }
			const String&	 text() const { return _text; }

		private:
			KeyCode		_key;
			KeyModifier	_modifier;
			String		_text;
	};
};

#endif
