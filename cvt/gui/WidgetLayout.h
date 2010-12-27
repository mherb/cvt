#ifndef CVT_WIDGETLAYOUT_H
#define CVT_WIDGETLAYOUT_H

#include <stdint.h>
#include <cvt/math/Vector.h>
#include <cvt/geom/Rect.h>
#include <cvt/math/Math.h>

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
			void rect( Recti& r, const Recti& parentrect ) const;

		private:
			enum LayoutH { LAYOUT_HNONE, LAYOUT_HL, LAYOUT_HR, LAYOUT_HLR };
			enum LayoutV { LAYOUT_VNONE, LAYOUT_VT, LAYOUT_VB, LAYOUT_VTB };

			LayoutH _layouth;
			LayoutV _layoutv;
			Vector2<unsigned int> _horizontal;
			Vector2<unsigned int> _vertical;
	};

	inline WidgetLayout::WidgetLayout()
	{
		_layouth = LAYOUT_HNONE;
		_layoutv = LAYOUT_VNONE;
		_horizontal.set( 0, 0 );
		_vertical.set( 0, 0 );
	}

	inline void WidgetLayout::setAnchoredLeft( unsigned int marginleft, unsigned int width )
	{
		_layouth = LAYOUT_HL;
		_horizontal.set( marginleft, width );
	}

	inline void WidgetLayout::setAnchoredRight( unsigned int marginright, unsigned int width )
	{
		_layouth = LAYOUT_HR;
		_horizontal.set( marginright, width );
	}

	inline void WidgetLayout::setAnchoredLeftRight( unsigned int marginleft, unsigned int marginright )
	{
		_layouth = LAYOUT_HLR;
		_horizontal.set( marginleft, marginright );
	}

	inline void WidgetLayout::setAnchoredTop( unsigned int margintop, unsigned int height )
	{
		_layoutv = LAYOUT_VT;
		_vertical.set( margintop, height );
	}

	inline void WidgetLayout::setAnchoredBottom( unsigned int marginbottom, unsigned int height )
	{
		_layoutv = LAYOUT_VB;
		_vertical.set( marginbottom, height );
	}

	inline void WidgetLayout::setAnchoredTopBottom( unsigned int margintop, unsigned int marginbottom )
	{
		_layoutv = LAYOUT_VTB;
		_vertical.set( margintop, marginbottom );
	}

	inline void WidgetLayout::rect( Recti& rect, const Recti& prect ) const
	{
		switch( _layouth ) {
			case LAYOUT_HL:
				{
					rect.x = prect.x + _horizontal[ 0 ];
					rect.width = _horizontal[ 1 ];
				}
				break;
			case LAYOUT_HR:
				{
					rect.x = prect.x + prect.width - _horizontal[ 0 ] - _horizontal[ 1 ];
					rect.width = _horizontal[ 1 ];
				}
				break;
			case LAYOUT_HLR:
				{
					rect.x = prect.x + _horizontal[ 0 ];
					rect.width = prect.width - _horizontal[ 0 ] - _horizontal[ 1 ];
				}
				break;
			case LAYOUT_HNONE:
				break;
		}

		switch( _layoutv ) {
			case LAYOUT_VT:
				{
					rect.y = prect.y + _vertical[ 0 ];
					rect.height = _vertical[ 1 ];
				}
				break;
			case LAYOUT_VB:
				{
					rect.y = prect.y + prect.height - _vertical[ 0 ] - _vertical[ 1 ];
					rect.height = _vertical[ 1 ];
				}
				break;
			case LAYOUT_VTB:
				{
					rect.y = prect.y + _vertical[ 0 ];
					rect.height = prect.height - _vertical[ 0 ] - _vertical[ 1 ];
				}
				break;
			case LAYOUT_VNONE:
				break;
		}
	}

}

#endif
