#include "calcflow.h"

#include "cvt/math/Math.h"
#include "cvt/io/ImageIO.h"
#include "cvt/gfx/ifilter/ROFDenoise.h"
#include <cvt/vision/Flow.h>

#include "highgui.h"

#include <deque>
#include <iostream>

#define SF 0.8f
#define LAMBDA 50.0f
#define THETA 0.15f
#define NUMWARP 10
#define NUMROF 30

using namespace cvt;

static void zeroborder( Image* img )
{
	uint8_t* data;
	size_t stride, w, h, i, C;
	float* pdata;

	data =img->map();
	stride = img->stride();
	w = img->width();
	h = img->height();
	C = img->channels();

	pdata = ( float* ) data;
	i = w * C;
	while( i-- )
		*pdata++ = 0.0f;

	data += stride;
	h -= 2;
	while( h-- ) {
		pdata = ( float* ) data;
		i = C;
		while( i-- )
			*pdata++ = 0.0f;
		pdata = ( float* ) ( data + ( w - 1 ) * C * sizeof( float ) );
		i = C;
		while( i-- )
			*pdata++ = 0.0f;
		data += stride;
	}

	pdata = ( float* ) data;
	i = w * C;
	while( i-- )
		*pdata++ = 0.0f;

	img->unmap();
}

static void multadd3( Image& idst, const Image& isrc, Image& dx, Image& dy, float lambda )
{
	uint8_t* dst;
	uint8_t const* src1;
	uint8_t* src2;
	uint8_t* src3;
	size_t stridedst;
	size_t stridesrc1;
	size_t stridesrc2;
	size_t stridesrc3;
	float* pdst;
	float const* psrc1;
	float* psrc2;
	float* psrc3;
	size_t w, h;

	dst = idst.map();
	stridedst = idst.stride();
	src1 = isrc.map();
	stridesrc1 = isrc.stride();
	src2 = dx.map();
	stridesrc2 = dx.stride();
	src3 = dy.map();
	stridesrc3 = dy.stride();

	h = idst.height();
	while( h-- ) {
		pdst = ( float* ) dst;
		psrc1 = ( float* ) src1;
		psrc2 = ( float* ) src2;
		psrc3 = ( float* ) src3;

		w = idst.width() * idst.channels();
		while( w-- ) {
			*pdst++ = *psrc1++ + lambda * ( *psrc2++ + *psrc3++ );
		}

		dst += stridedst;
		src1 += stridesrc1;
		src2 += stridesrc2;
		src3 += stridesrc3;
	}
	idst.unmap();
	isrc.unmap();
	dx.unmap();
	dy.unmap();
}

static void edgeweight( Image& idst, Image& dx, Image& dy )
{
	uint8_t* dst;
	uint8_t const* src1;
	uint8_t* src2;
	uint8_t* src3;
	size_t stridedst;
	size_t stridesrc2;
	size_t stridesrc3;
	float* pdst;
	float* psrc2;
	float* psrc3;
	size_t w, h;

	dst = idst.map();
	stridedst = idst.stride();
	src2 = dx.map();
	stridesrc2 = dx.stride();
	src3 = dy.map();
	stridesrc3 = dy.stride();

	h = idst.height();
	while( h-- ) {
		pdst = ( float* ) dst;
		psrc2 = ( float* ) src2;
		psrc3 = ( float* ) src3;

		w = idst.width() * idst.channels();
		while( w-- ) {
			*pdst++ = 1.0f; //Math::max( 1.0f * Math::exp( -0.1f * ::powf( Math::sqrt( Math::sqr( *psrc2++ ) + Math::sqr( *psrc3++ ) ), 0.5f ) ), 1e-6f );
			/**pdst++ = 1.0f / ( 1.0f +  0.1f * Math::sqrt( Math::sqr( *psrc2++ ) + Math::sqr( *psrc3++ ) ));*/
		}

		dst += stridedst;
		src2 += stridesrc2;
		src3 += stridesrc3;
	}
	idst.unmap();
	dx.unmap();
	dy.unmap();
}

static void multadd2_th( Image& idst1, Image& idst2, Image& dx, Image& dy, Image& edgew, float taulambda )
{
	uint8_t* dst1;
	uint8_t* dst2;
	uint8_t* src1;
	uint8_t* src2;
	uint8_t* src3;
	size_t stridedst1;
	size_t stridedst2;
	size_t stridesrc1;
	size_t stridesrc2;
	size_t stridesrc3;
	float* pdst1;
	float* pdst2;
	float* psrc1;
	float* psrc2;
	float* psrc3;
	size_t w, h;
	float tmp1, tmp2, norm;

	dst1 = idst1.map();
	stridedst1 = idst1.stride();
	dst2 = idst2.map();
	stridedst2 = idst2.stride();
	src1 = dx.map();
	stridesrc1 = dx.stride();
	src2 = dy.map();
	stridesrc2 = dy.stride();
	src3 = edgew.map();
	stridesrc3 = edgew.stride();


	h = idst1.height();
	while( h-- ) {
		pdst1 = ( float* ) dst1;
		pdst2 = ( float* ) dst2;
		psrc1 = ( float* ) src1;
		psrc2 = ( float* ) src2;
		psrc3 = ( float* ) src3;

		w = idst1.width() * idst1.channels();
		while( w-- ) {
			tmp1 = *pdst1 + taulambda * ( *psrc1++ );
			tmp2 = *pdst2 + taulambda * ( *psrc2++ );
			tmp1 *= ( *psrc3++ );
			tmp2 *= ( *psrc3++ );
			norm = Math::max( 1.0f, Math::sqrt( tmp1 * tmp1 + tmp2 * tmp2 )  );
			*pdst1++ = tmp1 / norm;
			*pdst2++ = tmp2 / norm;
		}

		dst1 += stridedst1;
		dst2 += stridedst2;
		src1 += stridesrc1;
		src2 += stridesrc2;
		src3 += stridesrc3;
	}
	idst1.unmap();
	idst2.unmap();
	dx.unmap();
	dy.unmap();
	edgew.unmap();
}

void threshold( Image* idst, Image* isrc, Image* ig2, Image* it, Image* ix, Image* iy, Image* isrc0, float lambdatheta )
{
	uint8_t* dst;
	uint8_t* src;
	uint8_t* g2;
	uint8_t* dt;
	uint8_t* dx;
	uint8_t* dy;
	uint8_t* src0;
	size_t dstride;
	size_t sstride;
	size_t g2stride;
	size_t dtstride;
	size_t dxstride;
	size_t dystride;
	size_t src0stride;
	size_t w, h, i;
	float v;

	dst = idst->map();
	dstride = idst->stride();
	src = isrc->map();
	sstride = isrc->stride();
	g2 = ig2->map();
	g2stride = ig2->stride();
	dt = it->map();
	dtstride = it->stride();
	dx = ix->map();
	dxstride = ix->stride();
	dy = iy->map();
	dystride = iy->stride();
	src0 = isrc0->map();
	src0stride = isrc0->stride();

	w = idst->width();
	h = idst->height();

	while( h-- ) {
		float* psrc = ( float* ) src;
		float* pdst = ( float* ) dst;
		float* pg2 = ( float* ) g2;
		float* pdt = ( float* ) dt;
		float* pdx = ( float* ) dx;
		float* pdy = ( float* ) dy;
		float* psrc0 = ( float* ) src0;

		i = w;
		while( i-- ) {
			float th = lambdatheta * *pg2;
			v = *pdt++ + ( *psrc - *psrc0 ) * *pdx + ( *( psrc + 1 ) - *( psrc0 + 1 ) ) * *pdy;
			if( v < -th ) {
				*pdst++ = *psrc++ + lambdatheta * *pdx++;
				*pdst++ = *psrc++ + lambdatheta * *pdy++;
			} else if( v > th ) {
				*pdst++ = *psrc++ - lambdatheta * *pdx++;
				*pdst++ = *psrc++ - lambdatheta * *pdy++;
			} else {
				*pdst++ = *psrc++ - ( v * *pdx++ ) / *pg2;
				*pdst++ = *psrc++ - ( v * *pdy++ ) / *pg2;
			}
			pg2++;
			psrc0 += 2;
		}
		dst += dstride;
		src += sstride;
		g2 += g2stride;
		dt += dtstride;
		dx += dxstride;
		dy += dystride;
		src0 += src0stride;
	}

	idst->unmap();
	isrc->unmap();
	ig2->unmap();
	it->unmap();
	ix->unmap();
	iy->unmap();
	isrc0->unmap();
}

void tvl1( Image* u, Image* v, Image* px, Image* py, float lambda, float theta, Image* ig2, Image* it, Image* ix, Image* iy, Image* v0, Image* edgew, size_t iter )
{
#if 0
		Image kerndx( 5, 1, IOrder::GRAY, IType::FLOAT );
		Image kerndy( 1, 5, IOrder::GRAY, IType::FLOAT );
		Image kerndxrev( 5, 1, IOrder::GRAY, IType::FLOAT );
		Image kerndyrev( 1, 5, IOrder::GRAY, IType::FLOAT );

		{
			float* data;
			data = ( float* ) kerndx.data();
			*data++ =  0.1f;
			*data++ = -0.9f;
			*data++ =  0.9f;
			*data++ = -0.1f;
			*data++ =  0.0f;

			data = ( float* ) kerndy.scanline( 0 );
			*data++ =  0.1f;
			data = ( float* ) kerndy.scanline( 1 );
			*data++ = -0.9f;
			data = ( float* ) kerndy.scanline( 2 );
			*data++ =  0.9f;
			data = ( float* ) kerndy.scanline( 3 );
			*data++ = -0.1f;
			data = ( float* ) kerndy.scanline( 4 );
			*data++ =  0.0f;

			data = ( float* ) kerndxrev.data();
			*data++ =  0.0f;
			*data++ =  0.1f;
			*data++ = -0.9f;
			*data++ =  0.9f;
			*data++ = -0.1f;

			data = ( float* ) kerndyrev.scanline( 0 );
			*data++ =  0.0f;
			data = ( float* ) kerndyrev.scanline( 1 );
			*data++ =  0.1f;
			data = ( float* ) kerndyrev.scanline( 2 );
			*data++ = -0.9f;
			data = ( float* ) kerndyrev.scanline( 3 );
			*data++ =  0.9f;
			data = ( float* ) kerndyrev.scanline( 4 );
			*data++ = -0.1f;
		}
#else
	Image kerndx( 2, 1, IOrder::GRAY, IType::FLOAT );
	Image kerndy( 1, 2, IOrder::GRAY, IType::FLOAT );
	Image kerndxrev( 3, 1, IOrder::GRAY, IType::FLOAT );
	Image kerndyrev( 1, 3, IOrder::GRAY, IType::FLOAT );
	{
			float* data;
			uint8_t* base;

			data = ( float* ) kerndx.map();
			*data++ = -1.0f;
			*data++ =  1.0f;
//			*data++ =  0.0f;
			kerndx.unmap();

			base = kerndy.map();
			data = ( float* ) ( base );
			*data++ = -1.0f;
			data = ( float* ) ( base + kerndy.stride() );
			*data++ =  1.0f;
//			data = ( float* ) kerndy.scanline( 2 );
//			*data++ =  0.0f;
			kerndy.unmap();

			data = ( float* ) kerndxrev.map();
			*data++ =  0.0f;
			*data++ = -1.0f;
			*data++ =  1.0f;
			kerndxrev.unmap();

			base = kerndyrev.map();
			data = ( float* ) ( base );
			*data++ =  0.0f;
			data = ( float* ) ( base + kerndyrev.stride() );
			*data++ = -1.0f;
			data = ( float* ) ( base + kerndyrev.stride() * 2 );
			*data++ =  1.0f;
			kerndyrev.unmap();
		}
#endif
	Image dx( u->width(), u->height(), IOrder::GRAYALPHA, IType::FLOAT );
    Image dy( u->width(), u->height(), IOrder::GRAYALPHA, IType::FLOAT );

#define TAU 0.249f
//	v0->copy( *v );
	while( iter-- ) {
		threshold( v, u, ig2, it, ix, iy, v0, lambda * theta );
		u->convolve( dx, kerndx, false );
		u->convolve( dy, kerndy, false );
		multadd2_th( *px, *py, dx, dy, *edgew, 1.0f / ( 4.0f * theta + 0.01f ) );
		px->convolve( dx, kerndxrev, false );
		py->convolve( dy, kerndyrev, false );
		multadd3( *u, *v, dx, dy, theta );
	}
	threshold( v, u, ig2, it, ix, iy, v0, lambda * theta );
}

void warp( Image* u, Image* v, Image* px, Image* py, Image* img1, Image* img2, size_t iter )
{
	float* data;
	Image kerndx( 5, 1, IOrder::GRAY, IType::FLOAT );
	Image kerndy( 1, 5, IOrder::GRAY, IType::FLOAT );
	Image v0;

	/* FIXME: use -1 0 1 */
	data = ( float* ) kerndx.map();
	*data++ = -0.1f;
	*data++ =  0.9f;
	*data++ =  0.0f;
	*data++ = -0.9f;
	*data++ =  0.1f;
	kerndx.unmap();

	{
		uint8_t* base = kerndy.map();
		data = ( float* ) ( base + kerndy.stride() * 0 );
		*data++ = -0.1f;
		data = ( float* ) ( base + kerndy.stride() * 1 );
		*data++ =  0.9f;
		data = ( float* ) ( base + kerndy.stride() * 2 );
		*data++ =  0.0f;
		data = ( float* ) ( base + kerndy.stride() * 3 );
		*data++ = -0.9f;
		data = ( float* ) ( base + kerndy.stride() * 4 );
		*data++ =  0.1f;
		kerndy.unmap();
	}

	Image i1x( img1->width(), img1->height(), IOrder::GRAY, IType::FLOAT );
	Image i1y( img1->width(), img1->height(), IOrder::GRAY, IType::FLOAT );
	Image i2x( img2->width(), img2->height(), IOrder::GRAY, IType::FLOAT );
	Image i2y( img2->width(), img2->height(), IOrder::GRAY, IType::FLOAT );
	img1->convolve( i1x, kerndx, false );
	img1->convolve( i1y, kerndy, false );
	img2->convolve( i2x, kerndx, false );
	img2->convolve( i2y, kerndy, false );

	Image iw( img2->width(), img2->height(), IOrder::GRAY, IType::FLOAT );
	Image iwx( img2->width(), img2->height(), IOrder::GRAY, IType::FLOAT );
	Image iwy( img2->width(), img2->height(), IOrder::GRAY, IType::FLOAT );
	Image it( img1->width(), img1->height(), IOrder::GRAY, IType::FLOAT );
	Image igsqr( img1->width(), img1->height(), IOrder::GRAY, IType::FLOAT );

	Image edgew( img1->width(), img1->height(), IOrder::GRAY, IType::FLOAT );
	edgeweight( edgew, i2x, i2y );

	/* median filter v before warping */
	v0.reallocate( *v );

	while( iter-- ) {
		cvSmooth( v->iplimage(), v0.iplimage(), CV_MEDIAN, 5 );

		img2->warpBilinear( iw, v0 );
		i2x.warpBilinear( iwx, v0 );
		i2y.warpBilinear( iwy, v0 );

		/* dt */
		it.copy( iw );
		it.sub( *img1 );

		/* combine dx/dy of img1 and warped img2 */
		iwx.add( i1x );
		iwx.mul( 0.5f );
		iwy.add( i1y );
		iwy.mul( 0.5f );

		/* FIXME: correct all finite differences ... */
		zeroborder( &iwx );
		zeroborder( &iwy );
		zeroborder( &it );

		/* square gradient of dx + dy */
		igsqr.copy( iwx );
		igsqr.mul( iwx );
		{
			Image tmp( iwy );
			tmp.mul( iwy );
			igsqr.add( tmp );
			igsqr.add( 1e-6f );
		}

		//threshold( v, u, &igsqr, &it, &iwx, &iwy, &v0, LAMBDA * THETA );
		tvl1( u, v, px, py, LAMBDA, THETA, &igsqr, &it, &iwx, &iwy, &v0, &edgew, NUMROF );
		{
			Image x;
			Flow::colorCode( x, *u );
			cvShowImage( "U", x.iplimage() );
			Flow::colorCode( x, *v );
			cvShowImage( "V", x.iplimage() );
			for( int i = 0; i < 10; i++ )
				cvWaitKey( 15 );
		}
	}
}

void calcflow( Image& flow, Image& img1, Image& img2, Image* gt )
{
	IScaleFilterCubic sfp;
	std::deque<std::pair<Image*,Image*> > pylevel;
	Image* cimg1;
	Image* cimg2;
	ROFDenoise rof;
	Image* itmp = new Image();

	pylevel.push_front( std::make_pair<Image*,Image*>( &img1, &img2 ) );
	do {
		std::pair<Image*,Image*> prev = pylevel.front();
		cimg1 = new Image( ( size_t ) ( prev.first->width() * SF + 0.5f ), ( size_t ) ( prev.first->height() * SF + 0.5f ), IOrder::GRAY, IType::FLOAT );
		cimg2 = new Image( ( size_t ) ( prev.second->width() * SF + 0.5f ), ( size_t ) ( prev.second->height() * SF + 0.5f ), IOrder::GRAY, IType::FLOAT );
		prev.first->scale( *cimg1, ( size_t ) ( prev.first->width() * SF + 0.5f ), ( size_t ) ( prev.first->height() * SF + 0.5f ), sfp );
		prev.second->scale( *cimg2, ( size_t ) ( prev.second->width() * SF + 0.5f ), ( size_t ) ( prev.second->height() * SF + 0.5f ), sfp );

//		rof.apply( *itmp, *cimg1, 0.1f, 100 );
//		cimg1->mad( *itmp, -0.90f );
//		//cimg1->mul( 5.0f );
//		//		cimg1->add( Color( 0.0f, 0.0f, 0.0f, 1.0f ) );
//
//		rof.apply( *itmp, *cimg2, 0.1f, 100 );
//		cimg2->mad( *itmp, -0.90f );
		//cimg2->mul( 5.0f );
		//		cimg2->add( Color( 0.0f, 0.0f, 0.0f, 1.0f ) );

		//		ImageIO::savePNG( *cimg1, "out1.png" );
		//		ImageIO::savePNG( *cimg2, "out2.png" );
		//		sleep( 5 );

		pylevel.push_front( std::make_pair<Image*,Image*>( cimg1, cimg2 ) );
	} while( Math::min( cimg1->width(), cimg1->height() ) > 16 );
	delete itmp;

	Image* u = NULL;
	Image* v = NULL;
	Image* px = NULL;
	Image* py = NULL;

	while( !pylevel.empty() ) {
		std::pair<Image*,Image*> pair = pylevel.front();
		pylevel.pop_front();
		size_t w, h;
		w = pair.first->width();
		h = pair.first->height();
		if( !u && !v ) {
			u = new Image( w, h, IOrder::GRAYALPHA, IType::FLOAT );
			v = new Image( w, h, IOrder::GRAYALPHA, IType::FLOAT );
			px = new Image( w, h, IOrder::GRAYALPHA, IType::FLOAT );
			py = new Image( w, h, IOrder::GRAYALPHA, IType::FLOAT );
			u->fill( Color( 0.0f, 0.0f ) );
			v->fill( Color( 0.0f, 0.0f ) );
			px->fill( Color( 0.0f, 0.0f ) );
			py->fill( Color( 0.0f, 0.0f ) );
		} else {
			IScaleFilterBilinear sf;
			Image* unew = new Image( w, h, IOrder::GRAYALPHA, IType::FLOAT );
			Image* vnew = new Image( w, h, IOrder::GRAYALPHA, IType::FLOAT );
			Image* pxnew = new Image( w, h, IOrder::GRAYALPHA, IType::FLOAT );
			Image* pynew = new Image( w, h, IOrder::GRAYALPHA, IType::FLOAT );

			float scalex = ( float ) w / ( float ) u->width();
			float scaley = ( float ) h / ( float ) u->height();

			u->scale( *unew, w, h, sf );
			v->scale( *vnew, w, h, sf );
			px->scale( *pxnew, w, h, sf );
			py->scale( *pynew, w, h, sf );
			delete u;
			delete v;
			delete px;
			delete py;
			u = unew;
			v = vnew;
			px = pxnew;
			py = pynew;
			u->mul( Color( scalex, scaley ) );
			v->mul( Color( scalex, scaley ) );
		}
		warp( u, v, px, py, pair.first, pair.second, NUMWARP );
	}

	flow.reallocate( *u );
	flow.copy( *u );
	delete u;
	delete v;
	delete px;
	delete py;
}
