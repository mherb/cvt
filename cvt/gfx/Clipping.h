#ifndef CVT_CLIPPING_H
#define CVT_CLIPPING_H

#include <cvt/math/Vector.h>

namespace cvt {
	class Clipping {
		public:
			static bool clip( const Recti& rect, Vector2i& pt1, Vector2i& pt2 );

		private:
			Clipping();
			Clipping( const Clipping& );
	};

	inline bool Clipping::clip( const Recti& rect, Vector2i& pt1, Vector2i& pt2 )
	{
		bool p1[ 4 ], p2[ 4 ];
		bool in1, in2;
		int xend, yend;

		/* zero line */
		if( pt1.x == pt2.x && pt1.y == pt2.y )
			return false;

		xend = rect.x + rect.width;
		yend = rect.y + rect.height;

		p1[ 0 ] = ( pt1.x >= rect.x );
		p1[ 1 ] = ( pt1.x <= xend );
		p1[ 2 ] = ( pt1.y >= rect.y );
		p1[ 3 ] = ( pt1.y <= yend );

		p2[ 0 ] = ( pt2.x >= rect.x );
		p2[ 1 ] = ( pt2.x <= xend );
		p2[ 2 ] = ( pt2.y >= rect.y );
		p2[ 3 ] = ( pt2.y <= yend );

		/* both points inside */
		if( p1[ 0 ] && p1[ 1 ] && p1[ 2 ] && p1[ 3 ] &&
		    p2[ 0 ] && p2[ 1 ] && p2[ 2 ] && p2[ 3 ] )
			return true;

		/* both points outside and on the same side of one rect line */
		if( !( p1[ 0 ] || p2[ 0 ] ) ||
			!( p1[ 1 ] || p2[ 1 ] ) ||
			!( p1[ 2 ] || p2[ 2 ] ) ||
			!( p1[ 3 ] || p2[ 3 ] ) )
			return false;

		/* horizontal or vertical lines */
		if( pt1.y == pt2.y ) {
			std::cout << "HORIZONTAL" << std::endl;
			if( !p1[ 0 ] ) pt1.x = rect.x;
			else if( !p1[ 1 ] ) pt1.x = xend;
			if( !p2[ 0 ] ) pt2.x = rect.x;
			else if( !p2[ 1 ] ) pt2.x = xend;
			return true;
		} else if( pt1.x == pt2.x ) {
			std::cout << "VERTICAL" << std::endl;
			if( !p1[ 2 ] ) pt1.y = rect.y;
			else if( !p1[ 3 ] ) pt1.y = yend;
			if( !p2[ 2 ] ) pt2.y = rect.y;
			else if( !p2[ 3 ] ) pt2.y = yend;
			return true;
		}

		/* point 1*/
		if( !p1[ 0 ] ) { /* left of rect, clip to left*/
			pt1.y = pt2.y + ( ( pt1.y - pt2.y ) * ( rect.x - pt2.x ) ) / ( pt1.x - pt2.x );
			pt1.x = rect.x;
		} else if( !p1[ 1 ] ) { /* right of rect, clip to right*/
			pt1.y = pt2.y + ( ( pt1.y - pt2.y ) * ( xend - pt2.x ) ) / ( pt1.x - pt2.x );
			pt1.x = xend;
		}

		if( pt1.y < rect.y ) { /* above rect, clip to top */
			pt1.x = pt2.x + ( ( pt1.x - pt2.x ) * ( rect.y - pt2.y ) ) / ( pt1.y - pt2.y );
			pt1.y = rect.y;
		} else if( pt1.y > yend ) { /* below rect, clip to bottom */
			pt1.x = pt2.x + ( ( pt1.x - pt2.x ) * ( yend - pt2.y ) ) / ( pt1.y - pt2.y );
			pt1.y = yend;
		}

		/* zero line */
		if( pt1.x == pt2.x && pt1.y == pt2.y )
			return false;

		/* point 2*/
		if( !p2[ 0 ] ) { /* left of rect, clip to left*/
			pt2.y = pt1.y + ( ( pt2.y - pt1.y ) * ( rect.x - pt1.x ) ) / ( pt2.x - pt1.x );
			pt2.x = rect.x;
		} else if( !p2[ 1 ] ) { /* right of rect, clip to right*/
			pt2.y = pt1.y + ( ( pt2.y - pt1.y ) * ( xend - pt1.x ) ) / ( pt2.x - pt1.x );
			pt2.x = xend;
		}

		if( pt2.y < rect.y ) { /* above rect, clip to top */
			pt2.x = pt1.x + ( ( pt2.x - pt1.x ) * ( rect.y - pt1.y ) ) / ( pt2.y - pt1.y );
			pt2.y = rect.y;
		} else if( pt2.y > yend ) { /* below rect, clip to bottom */
			pt2.x = pt1.x + ( ( pt2.x - pt1.x ) * ( yend - pt1.y ) ) / ( pt2.y - pt1.y );
			pt2.y = yend;
		}

		return true;
	}
}

#endif
