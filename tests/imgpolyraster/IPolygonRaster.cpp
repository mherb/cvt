#include "IPolygonRaster.h"
#include <cvt/util/ScopedBuffer.h>
#include <cvt/gfx/IMapScoped.h>

namespace cvt {

	struct WindingMask8 {
		uint32_t mask;
		uint32_t composite[2];
	};

#define  COMPOSITE_MIX8( a )  ( a ) |= ( a ) >> 14; ( a ) |= ( a ) >> 4;
#define  NOHIGHBITS  0x7f7f7f7f
#define  HIGHBITS    0x80808080

#define FIXED_TO_SUB8(x) ( ( x.frac() ) >> 13 )
#define SUBUNITS8 8
#define FULL_COVER8 0xff

#define  windingmask8_record(_mask, _ySub, _dir) {\
	uint32_t*  pt = &( _mask ).composite[ (_ySub) >> 2 ];\
	uint32_t   t = *pt;\
	(_mask).mask |= ( 1U << (_ySub) );\
	*pt = (t + ( ( ( int ) _dir & 0x7f) << ( 8 * ( (_ySub) & 0x03) ) ) ) & NOHIGHBITS;\
} while(0)

inline void windingmask8_reset( WindingMask8* dst, WindingMask8* src )
{
    uint32_t dmask = 0;
    uint32_t t;

    t = src->composite[ 0 ] & NOHIGHBITS;
    dst->composite[ 0 ] = t;
    src->composite[ 0 ] = 0;
    t = ( t + NOHIGHBITS ) & HIGHBITS;
    dmask |= ( t >> ( 7 ) );

    t = src->composite[ 1 ] & NOHIGHBITS;
    dst->composite[ 1 ] = t;
    src->composite[ 1 ] = 0;
    t = ( t + NOHIGHBITS ) & HIGHBITS;
    dmask |= ( t >> ( 6 ) );

    COMPOSITE_MIX8( dmask );

    dst->mask = dmask;
    src->mask = 0;
}

inline void windingmask8_apply( WindingMask8* dst, WindingMask8* src )
{
    uint32_t dmask = 0;
    uint32_t t, p;

    t = dst->composite[ 0 ] + NOHIGHBITS;
    p = ( dst->composite[ 0 ] + ( src->composite[ 0 ] & NOHIGHBITS ) ) & NOHIGHBITS;
    src->composite[ 0 ] = 0;
    dst->composite[ 0 ] = p;
    p += NOHIGHBITS;
    t  = ( t ^ p ) & HIGHBITS;
    dmask |= ( t >> ( 7 ) );

    t = dst->composite[ 1 ] + NOHIGHBITS;
    p = ( dst->composite[ 1 ] + ( src->composite[ 1 ] & NOHIGHBITS ) ) & NOHIGHBITS;
    src->composite[ 1 ] = 0;
    dst->composite[ 1 ] = p;
    p += NOHIGHBITS;
    t  = ( t ^ p ) & HIGHBITS;
    dmask |= ( t >> ( 6 ) );

    COMPOSITE_MIX8( dmask );

    dst->mask ^= dmask;
    src->mask  = 0;
}

const Fixed IPolygonRaster::_offsets8[ 8 ] = {
	( 5.0f / 8.0f ),
	( 0.0f / 8.0f ),
	( 3.0f / 8.0f ),
	( 6.0f / 8.0f ),
	( 1.0f / 8.0f ),
	( 4.0f / 8.0f ),
	( 7.0f / 8.0f ),
	( 2.0f / 8.0f )
};

void IPolygonRaster::addEdge( const Vector2f& pt1, const Vector2f& pt2 )
{
	if( pt1.x == pt2.x && pt1.y == pt2.y ) /*don't add zero-edges*/
		return;


	if( _edges.isEmpty() ) {
		_bounds = Rectf( pt1, pt2 );
	} else {
		_bounds.join( Rectf( pt1, pt2 ) );
	}
	_edges.append( PolyEdge( pt1, pt2 ) );
}



void IPolygonRaster::addEdgeClipRight( Vector2f& pt1, Vector2f& pt2 )
{
	if( pt1.x > _right && pt2.x > _right )
		return;

	float dy = pt2.y - pt1.y;

	if( pt2.x > _right ) {
		if( dy == 0 ) {
			pt2.x = _right;
		} else {
			float slope = dy / ( pt2.x - pt1.x ); /*dy !=0 -> both on one side of top-edge*/
			pt2.y = pt1.y + ( _right - pt1.x ) * slope;
			pt2.x = _right;
		}
		if( _hasCRight ) {
			_hasCRight = false;
			addEdge( pt2, _cright );
		} else {
			_hasCRight = true;
			_cright = pt2;
		}
	} else if( pt1.x > _right ) {
		if( dy == 0 ) {
			pt1.x = _right;
		} else {
			float slope = dy / ( pt2.x - pt1.x ); /*dy !=0 -> both on one side of top-edge*/
			pt1.y = pt2.y + ( _right - pt2.x ) * slope;
			pt1.x = _right;
		}
		if( _hasCRight ) {
			_hasCRight = false;
			addEdge( _cright, pt1 );
		} else {
			_hasCRight = true;
			_cright = pt1;
		}
	}
	addEdge( pt1, pt2 );
}



void IPolygonRaster::addEdgeClipBottom( Vector2f& pt1, Vector2f& pt2 )
{
	if( pt1.y > _bot && pt2.y > _bot )
		return;

	float dx = pt2.x - pt1.x;

	if( pt2.y > _bot ) {
		if( dx == 0 ) {
			pt2.y = _bot;
		} else {
			float slope = dx / ( pt2.y - pt1.y ); /*dy !=0 -> both on one side of top-edge*/
			pt2.x = pt1.x + ( _bot - pt1.y ) * slope;
			pt2.y = _bot;
		}
		if( _hasCBot ) {
			_hasCBot = false;
			addEdgeClipRight( pt2, _cbot );
		} else {
			_hasCBot = true;
			_cbot = pt2;
		}
	} else if( pt1.y > _bot ) {
		if( dx == 0 ) {
			pt1.y = _bot;
		} else {
			float slope = dx / ( pt2.y - pt1.y ); /*dy !=0 -> both on one side of top-edge*/
			pt1.x = pt2.x + ( _bot - pt2.y ) * slope;
			pt1.y = _bot;
		}
		if( _hasCBot ) {
			_hasCBot = false;
			addEdgeClipRight( _cbot, pt1 );
		} else {
			_hasCBot = true;
			_cbot = pt1;
		}
	}
	addEdgeClipRight( pt1, pt2 );
}



void IPolygonRaster::addEdgeClipLeft( Vector2f& pt1, Vector2f& pt2 )
{
	if( pt1.x < _left && pt2.x < _left )
		return;

	float dy = pt2.y - pt1.y;

	if( pt2.x < _left ) {
		if( dy == 0 ) {
			pt2.x = _left;
		} else {
			float slope = dy / ( pt2.x - pt1.x ); /*dy !=0 -> both on one side of top-edge*/
			pt2.y = pt1.y + ( _left - pt1.x ) * slope;
			pt2.x = _left;
		}
		if( _hasCLeft ) {
			_hasCLeft = false;
			addEdgeClipBottom( pt2, _cleft );
		} else {
			_hasCLeft = true;
			_cleft = pt2;
		}
	} else if( pt1.x < _left ) {
		if( dy == 0 ) {
			pt1.x = _left;
		} else {
			float slope = dy / ( pt2.x - pt1.x ); /*dy !=0 -> both on one side of top-edge*/
			pt1.y = pt2.y + ( _left - pt2.x ) * slope;
			pt1.x = _left;
		}
		if( _hasCLeft ) {
			_hasCLeft = false;
			addEdgeClipBottom( _cleft, pt1 );
		} else {
			_hasCLeft = true;
			_cleft = pt1;
		}
	}
	addEdgeClipBottom( pt1, pt2 );
}


void IPolygonRaster::addEdgeClipTop( Vector2f& pt1, Vector2f& pt2 )
{
	if( pt1.y < _top && pt2.y < _top )
		return;

	float dx = pt2.x - pt1.x;

	if( pt2.y < _top ) {
		if( dx == 0 ) {
			pt2.y = _top;
		} else {
			float slope = dx / ( pt2.y - pt1.y ); /*dy !=0 -> both on one side of top-edge*/
			pt2.x = pt1.x + ( _top - pt1.y ) * slope;
			pt2.y = _top;
		}
		if( _hasCTop ) {
			_hasCTop = false;
			addEdgeClipLeft( pt2, _ctop );
		} else {
			_hasCTop = true;
			_ctop = pt2;
		}
	} else if( pt1.y < _top ) {
		if( dx == 0 ) {
			pt1.y = _top;
		} else {
			float slope = dx / ( pt2.y - pt1.y ); /*dy !=0 -> both on one side of top-edge*/
			pt1.x = pt2.x + ( _top - pt2.y ) * slope;
			pt1.y = _top;
		}
		if( _hasCTop ) {
			_hasCTop = false;
			addEdgeClipLeft( _ctop, pt1 );
		} else {
			_hasCTop = true;
			_ctop = pt1;
		}
	}
	addEdgeClipLeft( pt1, pt2 );
}

void IPolygonRaster::addEdgeClip( const Vector2f& pt1, const Vector2f& pt2 )
{
	if( _cliprect.contains( pt1 ) && _cliprect.contains( pt2 ) ) {
		addEdge( pt1, pt2 );
	} else {
		Vector2f cpt1( pt1 ), cpt2( pt2 );
		addEdgeClipTop( cpt1, cpt2 );
	}
}


void IPolygonRaster::rasterize( Image& dst, const Color& color )
{
	rasterizeWinding( dst, color );
}

void IPolygonRaster::rasterizeEvenOdd( Image& imgdst, const Color& color )
{
	int i,cy,cx;
	List<PolyEdge>::Iterator it, etit;
	uint32_t mask;
	uint32_t *mptr,*mend;
	uint32_t *dst;
	int32_t maskwidth;
	int minx, maxx, xoffset;
	IMapScoped<uint32_t> map( imgdst );
	Fixed fxoffset;


	if(!_edges.size() || _bounds.isEmpty() )
		return;

	ScopedBuffer<uint32_t,true> smaskbuf( Math::ceil( _bounds.width ) + 2 );
	uint32_t* maskbuf = smaskbuf.ptr();

	maskwidth = Math::ceil( _bounds.width ) + 1;
	mend = &maskbuf[ maskwidth ];

	map.setLine( Math::floor( _bounds.y ) );
	xoffset = Math::floor( _bounds.x );
	fxoffset = Fixed( _bounds.x );
	fxoffset.native() &= ( ~0xffff );

	List<PolyEdge> aet;
	etit = _edges.begin();
	cy = etit->pt1.y.floor();

	while( etit != _edges.end() || !aet.isEmpty() ) {
		minx = maskwidth;
		maxx = 0;

		it = aet.begin();

		/*process active edges*/
		while( it != aet.end() ){
			Fixed fcx;
			mask = 1;

			PolyEdge& edge = *it;
			fcx = edge.cx;
			cx = fcx.floor();

			if( edge.pt2.y.floor() == cy ) {
				int suby;

				suby= FIXED_TO_SUB8( edge.pt2.y );
				if(suby > 0) {
					if( cx > maxx ) maxx = cx;
					if( cx < minx ) minx = cx;

					for(i=0;i<suby;i++) {
						cx = ( fcx + _offsets8[ i ] ).floor();
						maskbuf[ cx ] ^= mask;
						mask <<= 1;
						fcx += edge.subslope;
					}

					cx = ( fcx - edge.subslope ).floor();
					if( cx > maxx ) maxx = cx;
					if( cx < minx ) minx = cx;
				}
				it = aet.remove( it );
			} else {
				if( cx > maxx ) maxx = cx;
				if( cx < minx ) minx = cx;

				cx = ( fcx + _offsets8[ 0 ] ).floor();
				maskbuf[ cx ] ^= mask;
				mask <<= 1;
				fcx += edge.subslope;
				cx = ( fcx + _offsets8[ 1 ] ).floor();
				maskbuf[ cx ] ^= mask;
				mask <<= 1;
				fcx += edge.subslope;
				cx = ( fcx + _offsets8[ 2 ] ).floor();
				maskbuf[ cx ] ^= mask;
				mask <<= 1;
				fcx += edge.subslope;
				cx = ( fcx + _offsets8[ 3 ] ).floor();
				maskbuf[ cx ] ^= mask;
				mask <<= 1;
				fcx += edge.subslope;
				cx = ( fcx + _offsets8[ 4 ] ).floor();
				maskbuf[ cx ] ^= mask;
				mask <<= 1;
				fcx += edge.subslope;
				cx = ( fcx + _offsets8[ 5 ] ).floor();
				maskbuf[ cx ] ^= mask;
				mask <<= 1;
				fcx += edge.subslope;
				cx = ( fcx + _offsets8[ 6 ] ).floor();
				maskbuf[ cx ] ^= mask;
				mask <<= 1;
				fcx += edge.subslope;
				cx = ( fcx + _offsets8[ 7 ] ).floor();
				maskbuf[ cx ] ^= mask;

				cx = fcx.floor();
				if( cx > maxx ) maxx = cx;
				if( cx < minx ) minx = cx;
				edge.cx += edge.slope;
				it++;
			}
		}

		/*process edges beginning in the current scanline*/
		while( etit != _edges.end()  ) {
			Fixed fcx;
			int suby, sube;

			if( etit->pt1.y.floor() > cy )
				break;

			PolyEdge& edge = *etit;
			if( edge.pt1.y != edge.pt2.y ) {
				edge.cx = ( edge.pt1.x - fxoffset );

				fcx = edge.cx;
				cx = fcx.floor();

				if( edge.pt1.y.floor() == edge.pt2.y.floor() ){

					suby = FIXED_TO_SUB8( edge.pt1.y );
					sube = FIXED_TO_SUB8( edge.pt2.y );
					mask = 1 << suby;

					if(suby < sube ) {
						if( cx > maxx ) maxx = cx;
						if( cx < minx ) minx = cx;

						for( i = suby; i < sube; i++ ) {
							cx = ( fcx + _offsets8[i] ).floor();
							maskbuf[ cx ] ^= mask;
							mask <<= 1;
							fcx += edge.subslope;
						}

						cx = ( fcx - edge.subslope ).floor();
						if( cx > maxx ) maxx = cx;
						if( cx < minx ) minx = cx;
					}
				} else {
					suby = FIXED_TO_SUB8( edge.pt1.y );
					mask = 1 << suby;

					if(suby < SUBUNITS8) {
						if( cx > maxx ) maxx = cx;
						if( cx < minx ) minx = cx;

						for( i = suby; i < SUBUNITS8; i++ ) {
							cx = ( fcx + _offsets8[i] ).floor();
							maskbuf[ cx ] ^= mask;
							mask <<= 1;
							fcx += edge.subslope;
						}

						cx = ( fcx - edge.subslope ).floor();
						if( cx > maxx ) maxx = cx;
						if( cx < minx ) minx = cx;
					}
					Fixed tmp;
					tmp.native() = Fixed((int16_t)1).native() - edge.pt1.y.frac();
					edge.cx += edge.slope * tmp;
					//edge.cx = fcx - edge.subslope;
					aet.append( edge );
				}
			}
			etit++;
		}


		/*process current mask from minx to maxx+1*/
		mask = maskbuf[ minx ];
		mptr = &maskbuf[ minx ];
		*mptr++ = 0;
		dst = map.ptr() + xoffset + minx;
		mend = &maskbuf[ maxx + 1 ];
		*mend = FULL_COVER8;

		while( mptr <= mend )
		{
			if (mask == 0) {
				uint32_t* smp = mptr;
				do {
					mask = *mptr++;
				} while (mask == 0); 
				*( mptr - 1 ) = 0;
				dst += mptr - smp;
			} else if (mask == FULL_COVER8) {
				do {
				// FIXME
					*dst++ = 0xffffffff;
				} while(!*mptr++); 
				mask ^= *(mptr-1);
				*(mptr-1) = 0;
			} else {
				uint32_t alpha = ( ( Math::popcount(mask) * 0xff ) >> 3 );
				// FIXME
				uint32_t pixel = ( 0xff << 24 ) | ( alpha << 16 ) | ( alpha << 8 ) | alpha;

				do {
					// FIXME
					*dst++ = pixel;
					if( *mptr ) {
						mask ^= *mptr;
						alpha = ( ( Math::popcount( mask ) * 0xff ) >> 3 );
						// FIXME
						pixel = ( 0xff << 24 ) | ( alpha << 16 ) | ( alpha << 8 ) | alpha;
					}
					*mptr++ = 0;
				} while (! ( mask == 0 || mask == FULL_COVER8 ) && mptr <= mend );
			}
		}
		cy++;
		map++;
	}
}

void IPolygonRaster::rasterizeWinding( Image& imgdst, const Color& color )
{
	int i, cy, cx;
	List<PolyEdge>::Iterator it, etit;
	WindingMask8 mask[ 1 ];
	WindingMask8 *mptr, *mend, *temp;
	uint32_t *dst, *tmpdst;
	int32_t maskwidth;
	int minx, maxx, xoffset;
	IMapScoped<uint32_t> map( imgdst );
	Fixed fxoffset;

	if(!_edges.size() || _bounds.isEmpty() )
		return;

	ScopedBuffer<WindingMask8,true> smaskbuf( Math::ceil( _bounds.width ) + 2 );
	WindingMask8* maskbuf = smaskbuf.ptr();

	maskwidth = Math::ceil( _bounds.width ) + 1;
	mend = &maskbuf[ maskwidth ];

	map.setLine( Math::floor( _bounds.y ) );
	xoffset = Math::floor( _bounds.x );
	fxoffset = Fixed( _bounds.x );
	fxoffset.native() &= ( ~0xffff );

	List<PolyEdge> aet;
	etit = _edges.begin();
	cy = etit->pt1.y.floor();

	while( etit != _edges.end() || !aet.isEmpty() ){
		minx = maskwidth;
		maxx = 0;

		it = aet.begin();

		while( it != aet.end() ){
			Fixed fcx;

			PolyEdge& edge = *it;
			fcx =  edge.cx;
			cx = fcx.floor();

			if(  edge.pt2.y.floor() == cy) { /*edge ends in the current scanline*/
				int suby;

				suby = FIXED_TO_SUB8( edge.pt2.y );
				if( suby > 0 ) {
					if( cx > maxx ) maxx = cx;
					if( cx < minx ) minx = cx;

					for( i = 0 ; i < suby; i++ ) {
						cx = ( fcx + _offsets8[i] ).floor();
						windingmask8_record( maskbuf[ cx ], i, edge.dir );
						fcx +=  edge.subslope;
					}

					cx = ( fcx - edge.subslope ).floor();
					if( cx > maxx ) maxx = cx;
					if( cx < minx ) minx = cx;
				}
				it = aet.remove( it );
			} else { /*edge crosses complete scanline*/
				if( cx > maxx ) maxx = cx;
				if( cx < minx ) minx = cx;

				cx = ( fcx + _offsets8[ 0 ] ).floor();
				windingmask8_record(maskbuf[ cx ],0, edge.dir);
				fcx +=  edge.subslope;

				cx = ( fcx + _offsets8[ 1 ] ).floor();
				windingmask8_record(maskbuf[ cx ], 1, edge.dir);
				fcx +=  edge.subslope;

				cx = ( fcx + _offsets8[ 2 ] ).floor();
				windingmask8_record(maskbuf[ cx ], 2, edge.dir);
				fcx +=  edge.subslope;

				cx = ( fcx + _offsets8[ 3 ] ).floor();
				windingmask8_record(maskbuf[ cx ], 3, edge.dir);
				fcx +=  edge.subslope;

				cx = ( fcx + _offsets8[ 4 ] ).floor();
				windingmask8_record(maskbuf[ cx ], 4, edge.dir);
				fcx +=  edge.subslope;

				cx = ( fcx + _offsets8[ 5 ] ).floor();
				windingmask8_record(maskbuf[ cx ], 5, edge.dir);
				fcx +=  edge.subslope;

				cx = ( fcx + _offsets8[ 6 ] ).floor();
				windingmask8_record(maskbuf[ cx ], 6, edge.dir);
				fcx +=  edge.subslope;

				cx = ( fcx + _offsets8[ 7 ] ).floor();
				windingmask8_record(maskbuf[ cx ], 7, edge.dir);

				cx = fcx.floor();
				if( cx > maxx ) maxx = cx;
				if( cx < minx ) minx = cx;
				edge.cx += edge.slope;
				it++;
			}
		}

		/*add edges starting in the current scanline*/
		while( etit != _edges.end()  ) {
			Fixed fcx;
			int suby, sube;

			if( etit->pt1.y.floor() > cy )
				break;

			PolyEdge& edge = *etit;
			if( edge.pt1.y !=  edge.pt2.y ){
				edge.cx = ( edge.pt1.x - fxoffset );

				fcx= edge.cx;
				cx = fcx.floor();

				/*edge starts and ends in the current scanline*/
				if( edge.pt1.y.floor() == edge.pt2.y.floor() ){

					suby = FIXED_TO_SUB8( edge.pt1.y );
					sube = FIXED_TO_SUB8( edge.pt2.y );

					if(suby < sube) {
						if( cx > maxx ) maxx = cx;
						if( cx < minx ) minx = cx;

						for( i = suby; i < sube; i++ ) {
							cx = ( fcx + _offsets8[ i ] ).floor();
							windingmask8_record(maskbuf[ cx ], i, edge.dir );
							fcx += edge.subslope;
						}

						cx = ( fcx - edge.subslope ).floor();
						if( cx > maxx ) maxx = cx;
						if( cx < minx ) minx = cx;
					}
				} else {
					suby = FIXED_TO_SUB8( edge.pt1.y );

					if(suby < SUBUNITS8) {
						if( cx > maxx ) maxx = cx;
						if( cx < minx ) minx = cx;

						for( i = suby; i < SUBUNITS8; i++ ) {
							cx = ( fcx + _offsets8[i] ).floor();
							windingmask8_record( maskbuf[ cx ], i, edge.dir );
							fcx += edge.subslope;
						}

						cx = ( fcx - edge.subslope ).floor();
						if( cx > maxx ) maxx = cx;
						if( cx < minx ) minx = cx;
					}
					Fixed tmp;
					tmp.native() = Fixed((int16_t)1).native() - edge.pt1.y.frac();
					edge.cx += edge.slope * tmp;
					//edge.cx = fcx - edge.subslope;
					aet.append( edge );
				}
			}
			etit++;
		}

		mptr = &maskbuf[ minx ];
		mend = &maskbuf[ maxx + 1 ];
		mend->mask = FULL_COVER8;
		temp = mptr++;
		windingmask8_reset(mask,temp);
		dst = map.ptr() + xoffset + minx;

		while (mptr <= mend) {
			if (mask->mask == 0){
				WindingMask8* smp = mptr;
				do {
					temp = mptr++;
				} while (temp->mask == 0);
				dst += mptr - smp;
				windingmask8_reset(mask, temp);
			} else if ( mask->mask == FULL_COVER8 ) {
				do {
					// FIXME
					*dst++ = 0xffffffff;
					mptr++;
				} while( !mptr->mask );
				windingmask8_apply( mask, mptr );
			} else {
				uint32_t alpha = ( ( Math::popcount( mask->mask & 0xff ) * 0xff ) >> 3 );
				// FIXME
				uint32_t pixel = ( 0xff << 24 ) | ( alpha << 16 ) | ( alpha << 8 ) | alpha;

				do {
					// FIXME
					*dst++ = pixel;
					temp  = mptr++;
					if ( temp->mask ) {
						windingmask8_apply(mask, temp );
						alpha = ( ( Math::popcount( mask->mask & 0xff ) * 0xff ) >> 3 );
						pixel = ( 0xff << 24 ) | ( alpha << 16 ) | ( alpha << 8 ) | alpha;
					}
				} while (mask->mask != 0 && mask->mask != FULL_COVER8 && mptr <= mend);
			}
		}
		cy++;
		map++;
	}
}
}
