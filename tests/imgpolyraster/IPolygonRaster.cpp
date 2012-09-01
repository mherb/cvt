#include "IPolygonRaster.h"
#include <cvt/util/ScopedBuffer.h>
#include <cvt/gfx/IMapScoped.h>

namespace cvt {

	struct WindingMask8 {
		uint32_t mask;
		uint32_t composite[2];
	};

#define  NOHIGHBITS  0x7f7f7f7f
#define  HIGHBITS    0x80808080

#define FIXED_TO_SUB8(x) ( ( x.native() ) >> 13 )
#define SUBUNITS8 8
#define FULL_COVER8 0xff

#define  _zwindingmask8_record(_mask, _ySub, _dir) {\
	uint32_t*  pt = &( _mask ).composite[ (_ySub) >> 2 ];\
	uint32_t   t = *pt;\
	(_mask).mask |= ( 1U << (_ySub) );\
	*pt = (t + ( ( ( int ) _dir & 0x7f) << ( 8 * ( (_ySub) & 0x03) ) ) ) & NOHIGHBITS;\
} while(0)


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
	rasterizeEvenOdd( dst, color );
}

#if 1
void IPolygonRaster::rasterizeEvenOdd( Image& imgdst, const Color& color )
{
	int i,cy,cx;
	List<PolyEdge>::Iterator it, etit;
	uint32_t mask;
	uint32_t *mptr,*mend;
	uint32_t *dst;
	uint32_t maskwidth;
	int minx, maxx, xoffset;
	IMapScoped<uint32_t> map( imgdst );


	if(!_edges.size() || _bounds.isEmpty() )
		return;

	ScopedBuffer<uint32_t,true> smaskbuf( Math::ceil( _bounds.width ) + 2 );
	uint32_t* maskbuf = smaskbuf.ptr();
	SIMD::instance()->SetValueU32( maskbuf, 0, Math::ceil( _bounds.width ) + 2 );

	maskwidth = Math::ceil( _bounds.width ) + 1;
	mend = &maskbuf[ maskwidth ];

	map.setLine( Math::floor( _bounds.y ) );
	xoffset = Math::floor( _bounds.x );

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
						cx = ( fcx + _offsets8[i] ).floor();
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
				maskbuf[cx] ^= mask;
				mask <<= 1;
				fcx += edge.subslope;
				cx = ( fcx + _offsets8[ 1 ] ).floor();
				maskbuf[cx] ^= mask;
				mask <<= 1;
				fcx += edge.subslope;
				cx = ( fcx + _offsets8[ 2 ] ).floor();
				maskbuf[cx] ^= mask;
				mask <<= 1;
				fcx += edge.subslope;
				cx = ( fcx + _offsets8[ 3 ] ).floor();
				maskbuf[cx] ^= mask;
				mask <<= 1;
				fcx += edge.subslope;
				cx = ( fcx + _offsets8[ 4 ] ).floor();
				maskbuf[cx] ^= mask;
				mask <<= 1;
				fcx += edge.subslope;
				cx = ( fcx + _offsets8[ 5 ] ).floor();
				maskbuf[cx] ^= mask;
				mask <<= 1;
				fcx += edge.subslope;
				cx = ( fcx + _offsets8[ 6 ] ).floor();
				maskbuf[cx] ^= mask;
				mask <<= 1;
				fcx += edge.subslope;
				cx = ( fcx + _offsets8[ 7 ] ).floor();
				maskbuf[cx] ^= mask;

				cx = fcx.floor();
				if( cx > maxx ) maxx = cx;
				if( cx < minx ) minx = cx;

				edge.cx += edge.slope;
				it++;
			}
		}

		/*process edges beginning in the current scanline*/
		while( etit != _edges.end() && etit->pt1.y.floor() == cy ) {
			Fixed fcx;
			int suby,sube;

			PolyEdge& edge = *etit;
			if( edge.pt1.y != edge.pt2.y ) {
				edge.cx = edge.pt1.x - ( Math::floor( _bounds.x ) );
				edge.subslope = edge.slope >> 3;

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
							maskbuf[cx] ^= mask;
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
							maskbuf[cx] ^= mask;
							mask <<= 1;
							fcx += edge.subslope;
						}

						cx = ( fcx - edge.subslope ).floor();
						if( cx > maxx ) maxx = cx;
						if( cx < minx ) minx = cx;
					}
					edge.cx = fcx;
					aet.append( edge );
				}
			}
			etit++;
		}

		/*process current mask from minx to maxx+1*/
		mask = maskbuf[ minx ];
		mptr = &maskbuf [minx ];
		*mptr++ = 0;
		dst = map.ptr() + xoffset + minx;
		mend = &maskbuf[ maxx + 1 ];
		*mend = FULL_COVER8;

		while (mptr <= mend)
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

#endif

#if 0
void zraster_polygonfx_aa8_winding(ZGFXImage* gfximg,ZPolygonfx* poly)
{
	int i,cy,cx;
	ZImage* img=gfximg->img;
	ZList* aet;
	ZPolyEdgefx* edge;
	ZListNode *it,*node,*etit;
	ZWindingMask8* maskbuf;
	ZWindingMask8 mask[1];
	ZWindingMask8 *mptr,*mend,*temp;
	uint32_t *dst,*tmpdst;
	ZColorPixel pcolor;
	uint32_t color;
	uint32_t maskwidth;
	int minx, maxx;

	zcolor_get_imagepixel(&gfximg->gfx.color, img->type, &pcolor);
	color = pcolor.c4_ub;

	if(!zlist_size(poly->edges) || !zfixed_floor(poly->bounds.width) || !zfixed_floor(poly->bounds.height) )
		return;

	maskbuf = zcalloc(zfixed_ceil(poly->bounds.width)+3,sizeof(ZWindingMask8));

	maskwidth = zfixed_ceil(poly->bounds.width)+1;
	mend = &maskbuf[maskwidth];

	dst=&((uint32_t*)img->pixel)[(img->stride>>2)*(gfximg->gfx.cbounds.y+zfixed_floor(poly->bounds.y)-gfximg->gfx.bounds.y)+(gfximg->gfx.cbounds.x+zfixed_floor(poly->bounds.x)-gfximg->gfx.bounds.x)];

	aet=zlist_new();
	etit=zlist_first(poly->edges);
	cy=zfixed_floor(((ZPolyEdgefx*)zlistnode_get_data(etit))->pt1.y);

	while(etit || zlist_size(aet)){

		minx = maskwidth;
		maxx = 0;

		it=zlist_first(aet);

		while(it){
			ZFixed fcx;
			node=zlistnode_next(it);
			edge=zlistnode_get_data(it);

			fcx=edge->cx;
			cx = zfixed_floor(fcx);

			if(zfixed_floor(edge->pt2.y)==cy){ /*edge ends in the current scanline*/
				int suby;

				suby=FIXED_TO_SUB8(edge->pt2.y);                
				if(suby > 0) {
					if( cx > maxx ) maxx = cx;
					if( cx < minx ) minx = cx;

					for(i=0;i<suby;i++){                    
						cx = zfixed_floor(fcx + _offsets8[i]);
						_zwindingmask8_record(maskbuf[cx],i,edge->dir);
						fcx += edge->subslope;
					}

					cx = zfixed_floor(fcx - edge->subslope);
					if( cx > maxx ) maxx = cx;
					if( cx < minx ) minx = cx;
				}
				zlist_delete(aet,it);
			} else { /*edge crosses complete scanline*/
				if( cx > maxx ) maxx = cx;
				if( cx < minx ) minx = cx;

				cx = zfixed_floor(fcx + _offsets8[0]);
				_zwindingmask8_record(maskbuf[cx],0,edge->dir); 
				fcx += edge->subslope;

				cx = zfixed_floor(fcx + _offsets8[1]);
				_zwindingmask8_record(maskbuf[cx],1,edge->dir); 
				fcx += edge->subslope;

				cx = zfixed_floor(fcx + _offsets8[2]);
				_zwindingmask8_record(maskbuf[cx],2,edge->dir); 
				fcx += edge->subslope;

				cx = zfixed_floor(fcx + _offsets8[3]);
				_zwindingmask8_record(maskbuf[cx],3,edge->dir); 
				fcx += edge->subslope;

				cx = zfixed_floor(fcx + _offsets8[4]);
				_zwindingmask8_record(maskbuf[cx],4,edge->dir); 
				fcx += edge->subslope;

				cx = zfixed_floor(fcx + _offsets8[5]);
				_zwindingmask8_record(maskbuf[cx],5,edge->dir); 
				fcx += edge->subslope;

				cx = zfixed_floor(fcx + _offsets8[6]);
				_zwindingmask8_record(maskbuf[cx],6,edge->dir); 
				fcx += edge->subslope;

				cx = zfixed_floor(fcx + _offsets8[7]);
				_zwindingmask8_record(maskbuf[cx],7,edge->dir); 

				cx = zfixed_floor(fcx);
				if( cx > maxx ) maxx = cx;
				if( cx < minx ) minx = cx;

				edge->cx+=edge->slope;
			}
			it=node;
		}

		/*add edges starting in the current scanline*/
		while(etit && zfixed_floor(((ZPolyEdgefx*)zlistnode_get_data(etit))->pt1.y) == cy) {
			ZFixed fcx;
			int suby,sube;

			edge=zlistnode_get_data(etit);
			if(edge->pt1.y != edge->pt2.y){
				edge->cx=(edge->pt1.x-(poly->bounds.x&(~0xffff)));
				edge->subslope = ((edge->slope)>>3);

				fcx=edge->cx;
				cx = zfixed_floor(fcx);

				/*edge starts and ends in the current scanline*/
				if(zfixed_floor(edge->pt1.y)==zfixed_floor(edge->pt2.y)){

					suby=FIXED_TO_SUB8(zfixed_frac(edge->pt1.y));
					sube=FIXED_TO_SUB8(zfixed_frac(edge->pt2.y));

					if(suby < sube) {
						if( cx > maxx ) maxx = cx;
						if( cx < minx ) minx = cx;

						for(i=suby;i<sube;i++){
							cx = zfixed_floor(fcx + _offsets8[i]);
							_zwindingmask8_record(maskbuf[cx],i,edge->dir);
							fcx += edge->subslope;
						}

						cx = zfixed_floor(fcx - edge->subslope);
						if( cx > maxx ) maxx = cx;
						if( cx < minx ) minx = cx;
					}
				} else {
					suby=FIXED_TO_SUB8(zfixed_frac(edge->pt1.y));

					if(suby < SUBUNITS8) {
						if( cx > maxx ) maxx = cx;
						if( cx < minx ) minx = cx;

						for(i=suby;i<SUBUNITS8;i++){
							cx = zfixed_floor(fcx + _offsets8[i]);
							_zwindingmask8_record(maskbuf[cx],i,edge->dir);
							fcx += edge->subslope;
						}

						cx = zfixed_floor(fcx - edge->subslope);
						if( cx > maxx ) maxx = cx;
						if( cx < minx ) minx = cx;
					}
					edge->cx+=zfixed_mul(edge->slope,(zfixed_from_int(1)-(zfixed_frac(edge->pt1.y))));
					zlist_append(aet,edge);
				}
			}
			etit=zlistnode_next(etit);
		}

		mptr = &maskbuf[minx];
		mend = &maskbuf[maxx+1];
		mend->mask = FULL_COVER8;
		tmpdst = dst;
		dst += minx;
		temp = mptr++;
		_zwindingmask8_reset(mask,temp);

		while (mptr <= mend) {
			if (mask->mask == 0){
				ZWindingMask8* smp = mptr;
				do {
					temp = mptr++;
				} while (temp->mask == 0);
				dst += mptr - smp;
				_zwindingmask8_reset(mask, temp);
			} else if (mask->mask == FULL_COVER8){       
				do {
					ZPIXEL_PBGRA_UB_OVER(*dst, *dst, color);
					dst++;
					mptr++;
				} while(!mptr->mask);
				_zwindingmask8_apply(mask, mptr);
			} else {
				uint32_t alpha =((ztable_bitcount(mask->mask&0xff)*0xff)>>3);
				uint32_t pixel;

				ZPIXEL_C4_UB_MUL(pixel, color, alpha);
				do {
					ZPIXEL_PBGRA_UB_OVER(*dst, *dst, pixel);
					dst++;
					temp  = mptr++;
					if (temp->mask){
						_zwindingmask8_apply(mask, temp );
						alpha = ((ztable_bitcount(mask->mask&0xff)*0xff)>>3);
						ZPIXEL_C4_UB_MUL(pixel, color, alpha);
					}
				} while (mask->mask != 0 && mask->mask != FULL_COVER8 && mptr <= mend);
			}
		}
		cy++;
		dst=tmpdst+(img->stride>>2);
	}

	zfree(maskbuf);
	zlist_destroy(aet);
}
#endif
}
