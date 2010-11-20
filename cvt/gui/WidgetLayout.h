#ifndef CVT_WIDGETLAYOUT_H
#define CVT_WIDGETLAYOUT_H

#include <stdint.h>
#include <cvt/math/Vector.h>
#include <cvt/util/Rect.h>
#include <cvt/util/Math.h>

namespace cvt {

	class WidgetLayout
	{
		public:
			WidgetLayout();

			void setAnchoredLeft( unsigned int marginleft, unsigned int width );
			void setAnchoredRight( unsigned int marginright, unsigned int width );
			void setAnchoredLeftRight( unsigned int marginleft, unsigned int marginright );
			void setAnchoredTop( unsigned int margintop, unsigned int height );
			void setAnchoredBottom( unsigned int marginbottom, unsigned int height );
			void setAnchoredTopBottom( unsigned int margintop, unsigned int marginbottom );
			void rect( Recti& r, unsigned int w, unsigned int h ) const;

		private:
			enum LayoutH { LAYOUT_HL, LAYOUT_HR, LAYOUT_HLR };
			enum LayoutV { LAYOUT_VT, LAYOUT_VB, LAYOUT_VTB };

			LayoutH _layouth;
			LayoutV _layoutv;
			Vector2<unsigned int> _horizontal;
			Vector2<unsigned int> _vertical;
	};

	inline WidgetLayout::WidgetLayout()
	{
		_layouth = LAYOUT_HL;
		_layoutv = LAYOUT_VT;
		_horizontal.set( 0, 10 );
		_vertical.set( 0, 10 );
	}

	void WidgetLayout::setAnchoredLeft( unsigned int marginleft, unsigned int width )
	{
		_layouth = LAYOUT_HL;
		_horizontal.set( marginleft, width );
	}

	void WidgetLayout::setAnchoredRight( unsigned int marginright, unsigned int width )
	{
		_layouth = LAYOUT_HR;
		_horizontal.set( marginright, width );
	}

	void WidgetLayout::setAnchoredLeftRight( unsigned int marginleft, unsigned int marginright )
	{
		_layouth = LAYOUT_HLR;
		_horizontal.set( marginleft, marginright );
	}

	void WidgetLayout::setAnchoredTop( unsigned int margintop, unsigned int height )
	{
		_layoutv = LAYOUT_VT;
		_horizontal.set( margintop, height );
	}

	void WidgetLayout::setAnchoredBottom( unsigned int marginbottom, unsigned int height )
	{
		_layoutv = LAYOUT_VB;
		_horizontal.set( marginbottom, height );
	}

	void WidgetLayout::setAnchoredTopBottom( unsigned int margintop, unsigned int marginbottom )
	{
		_layoutv = LAYOUT_VTB;
		_horizontal.set( margintop, marginbottom );
	}

	void WidgetLayout::rect( Recti& r, int w, int h ) const
	{
		switch( _layouth ) {
			case LAYOUT_HL:
				{
					rect.x = _vertical[ 0 ];
					rect.width = _vertical[ 1 ];
				}
				break;
			case LAYOUT_HR:
				{
					rect.x = w - _vertical[ 0 ] - _vertical[ 1 ];
					rect.width = _vertical[ 1 ];
				}
				break;
			case LAYOUT_HLR:
				{
					rect.x = _vertical[ 0 ];
					rect.width = w - _vertical[ 0 ] - _vertical[ 1 ];
				}
				break;
		}

		switch( _layoutv ) {
			case LAYOUT_VT:
				{
					rect.y = _vertical[ 0 ];
					rect.height = _vertical[ 1 ];
				}
				break;
			case LAYOUT_VB:
				{
					rect.y = h - _vertical[ 0 ] - _vertical[ 1 ];
					rect.height = _vertical[ 1 ];
				}
				break;
			case LAYOUT_VTB:
				{
					rect.y = _vertical[ 0 ];
					rect.height = h - _vertical[ 0 ] - _vertical[ 1 ];
				}
				break;
		}
	}

}

#endif
