/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/gui/ToggleButton.h>

namespace cvt {
	ToggleButton::ToggleButton( const String& label, bool state, GFX::Icon off, GFX::Icon on ) : _label( label ), _state( state ), _iconon( on ), _iconoff( off )
	{
	}

	ToggleButton::ToggleButton( bool state, GFX::Icon off, GFX::Icon on ) : _state( state ), _iconon( on ), _iconoff( off )
	{
	}

	ToggleButton::~ToggleButton()
	{

	}

	void ToggleButton::paintEvent( PaintEvent& , GFX& g )
	{
		int w, h;
		size( w, h );
		if( _label.isEmpty() ) {
			w = ( w - 16 ) >> 1;
			h = ( h - 16 ) >> 1;
			g.color().set( 0.6f, 0.6f, 0.6f, 1.0f );
			g.drawIcon( w, h, _state? _iconon : _iconoff );
		} else {
			w = 2;
			h = ( h - 16 ) >> 1;
			g.color().set( 0.6f, 0.6f, 0.6f, 1.0f );
			g.drawIcon( w, h, _state? _iconon : _iconoff );
			g.color().set( 0.8f, 0.8f, 0.8f, 1.0f );
			size( w, h );
			g.drawText( 20, 0, w - 20 , h, ALIGN_LEFT | ALIGN_VCENTER , _label.c_str() );
		}
	}

	void ToggleButton::mouseReleaseEvent( MouseReleaseEvent& event )
	{
		Recti self;
		self.setPosition( 0, 0 );
		size( self.width, self.height );
		if( self.contains( event.x, event.y ) && event.button() == 1 ) {
			_state = !_state;
			toggled.notify( this );
		}
		update();
	}

}
