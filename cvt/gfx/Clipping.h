#ifndef CVT_CLIPPING_H
#define CVT_CLIPPING_H

#include <cvt/math/Vector.h>

namespace cvt {
	class Clipping {
		public:
			static bool clip( const Recti& rect, Vector2i& pt1, Vector2i& pt2 );
			static bool clip( const Rectf& rect, Vector2f& pt1, Vector2f& pt2 );

		private:
			Clipping();
			Clipping( const Clipping& );
	};

	inline bool Clipping::clip( const Recti& rect, Vector2i& pt1, Vector2i& pt2 )
	{
		int xend, yend;
		int dx, dy, ox, oy;

		/* zero line */
		if( pt1.x == pt2.x && pt1.y == pt2.y )
			return false;

		/* both points inside */
		if( rect.contains( pt1 ) && rect.contains( pt2 ) )
			return true;

		xend = rect.x + rect.width;
		yend = rect.y + rect.height;

		/* both points outside and on the same side of one rect line */
		if( ( pt1.x < rect.x && pt2.x < rect.x ) ||
			( pt1.x > xend && pt2.x > xend ) ||
			( pt1.y < rect.y && pt2.y < rect.y ) ||
			( pt1.y > yend && pt2.y > yend ) )
			return false;

		/* horizontal or vertical lines */
		if( pt1.y == pt2.y ) {
			pt1.x = Math::clamp( pt1.x, rect.x, xend );
			pt2.x = Math::clamp( pt2.x, rect.x, xend );
			return true;
		} else if( pt1.x == pt2.x ) {
			pt1.y = Math::clamp( pt1.y, rect.y, yend );
			pt2.y = Math::clamp( pt2.y, rect.y, yend );
			return true;
		}

		dx = pt2.x - pt1.x;
		dy = pt2.y - pt1.y;
		ox = pt1.x;
		oy = pt1.y;

		/* point 1*/
		if( pt1.x < rect.x ) { /* left of rect, clip to left*/
			pt1.y = pt2.y + ( dy * ( rect.x - pt2.x ) ) / dx;
			pt1.x = rect.x;
		} else if( pt1.x > xend ) { /* right of rect, clip to right*/
			pt1.y = pt2.y + ( dy * ( xend - pt2.x ) ) / dx;
			pt1.x = xend;
		}

		if( pt1.y < rect.y ) { /* above rect, clip to top */
			pt1.x = Math::clamp( pt2.x + ( dx * ( rect.y - pt2.y ) ) / dy, rect.x, xend );
			pt1.y = rect.y;
		} else if( pt1.y > yend ) { /* below rect, clip to bottom */
			pt1.x = Math::clamp( pt2.x + ( dx  * ( yend - pt2.y ) ) / dy, rect.x, xend );
			pt1.y = yend;
		}

		/* zero line */
		if( pt1.x == pt2.x && pt1.y == pt2.y )
			return false;

		/* point 2*/
		if( pt2.x < rect.x ) { /* left of rect, clip to left*/
			pt2.y = oy + ( dy * ( rect.x - ox ) ) / dx;
			pt2.x = rect.x;
		} else if( pt2.x > xend ) { /* right of rect, clip to right*/
			pt2.y = oy + ( dy * ( xend - ox ) ) / dx;
			pt2.x = xend;
		}

		if( pt2.y < rect.y ) { /* above rect, clip to top */
			pt2.x = Math::clamp( ox + ( dx * ( rect.y - oy ) ) / dy, rect.x, xend );
			pt2.y = rect.y;
		} else if( pt2.y > yend ) { /* below rect, clip to bottom */
			pt2.x = Math::clamp( ox + ( dx * ( yend - oy ) ) / dy, rect.x, xend );
			pt2.y = yend;
		}

		/* zero line */
		if( pt1.x == pt2.x && pt1.y == pt2.y )
			return false;

		return true;
	}

	inline bool Clipping::clip( const Rectf& rect, Vector2f& pt1, Vector2f& pt2 )
	{
		float xend, yend;
		float dx, dy, ox, oy;

		/* zero line */
		if( pt1.x == pt2.x && pt1.y == pt2.y )
			return false;

		/* both points inside */
		if( rect.contains( pt1 ) && rect.contains( pt2 ) )
			return true;

		xend = rect.x + rect.width;
		yend = rect.y + rect.height;

		/* both points outside and on the same side of one rect line */
		if( ( pt1.x < rect.x && pt2.x < rect.x ) ||
			( pt1.x > xend && pt2.x > xend ) ||
			( pt1.y < rect.y && pt2.y < rect.y ) ||
			( pt1.y > yend && pt2.y > yend ) )
			return false;

		/* horizontal or vertical lines */
		if( pt1.y == pt2.y ) {
			pt1.x = Math::clamp( pt1.x, rect.x, xend );
			pt2.x = Math::clamp( pt2.x, rect.x, xend );
			return true;
		} else if( pt1.x == pt2.x ) {
			pt1.y = Math::clamp( pt1.y, rect.y, yend );
			pt2.y = Math::clamp( pt2.y, rect.y, yend );
			return true;
		}

		dx = pt2.x - pt1.x;
		dy = pt2.y - pt1.y;
		ox = pt1.x;
		oy = pt1.y;

		/* point 1*/
		if( pt1.x < rect.x ) { /* left of rect, clip to left*/
			pt1.y = pt2.y + ( dy * ( rect.x - pt2.x ) ) / dx;
			pt1.x = rect.x;
		} else if( pt1.x > xend ) { /* right of rect, clip to right*/
			pt1.y = pt2.y + ( dy * ( xend - pt2.x ) ) / dx;
			pt1.x = xend;
		}

		if( pt1.y < rect.y ) { /* above rect, clip to top */
			pt1.x = Math::clamp( pt2.x + ( dx * ( rect.y - pt2.y ) ) / dy, rect.x, xend );
			pt1.y = rect.y;
		} else if( pt1.y > yend ) { /* below rect, clip to bottom */
			pt1.x = Math::clamp( pt2.x + ( dx  * ( yend - pt2.y ) ) / dy, rect.x, xend );
			pt1.y = yend;
		}

		/* zero line */
		if( pt1.x == pt2.x && pt1.y == pt2.y )
			return false;

		/* point 2*/
		if( pt2.x < rect.x ) { /* left of rect, clip to left*/
			pt2.y = oy + ( dy * ( rect.x - ox ) ) / dx;
			pt2.x = rect.x;
		} else if( pt2.x > xend ) { /* right of rect, clip to right*/
			pt2.y = oy + ( dy * ( xend - ox ) ) / dx;
			pt2.x = xend;
		}

		if( pt2.y < rect.y ) { /* above rect, clip to top */
			pt2.x = Math::clamp( ox + ( dx * ( rect.y - oy ) ) / dy, rect.x, xend );
			pt2.y = rect.y;
		} else if( pt2.y > yend ) { /* below rect, clip to bottom */
			pt2.x = Math::clamp( ox + ( dx * ( yend - oy ) ) / dy, rect.x, xend );
			pt2.y = yend;
		}

		/* zero line */
		if( pt1.x == pt2.x && pt1.y == pt2.y )
			return false;

		return true;
	}
}

#endif
