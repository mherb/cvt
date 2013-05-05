/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_LABEL_H
#define CVT_LABEL_H

#include <cvt/gui/Widget.h>
#include <cvt/util/Signal.h>
#include <cvt/util/String.h>
#include <cvt/gfx/Alignment.h>

namespace cvt {
	class Label : public Widget
	{
		public:
			Label( const String& label, Alignment align = ALIGN_LEFT | ALIGN_VCENTER );
			~Label();
			const String& label() const;
			void setLabel( const String& label );
			Alignment labelAlignment() const;
			void setLabelAlignment( Alignment a );
			const Color& color() const;
			Color& color();

		private:
			void paintEvent( PaintEvent& e, GFX& g );
			Label( const Label& b );

			String _label;
			Alignment _aligment;
			Color _color;
	};

	inline const String& Label::label() const
	{
		return _label;
	}

	inline void Label::setLabel( const String& label )
	{
		_label = label;
		update();
	}

	inline Alignment Label::labelAlignment() const
	{
		return _aligment;
	}

	inline void Label::setLabelAlignment( Alignment a )
	{
		_aligment = a;
		update();
	}

	inline const Color& Label::color() const
	{
		return _color;
	}

	inline Color& Label::color()
	{
		return _color;
	}

}

#endif
