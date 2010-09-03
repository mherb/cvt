#include "gfx/ifilter/ROFDenoise.h"
#include "math/Math.h"

namespace cvt {

	static void multadd2( Image& idst, Image& dx, Image& dy, float lambda )
	{
		uint8_t* dst;
		uint8_t* src1;
		uint8_t* src2;
		size_t stridedst;
		size_t stridesrc1;
		size_t stridesrc2;
		float* pdst;
		float* psrc1;
		float* psrc2;
		size_t w, h;

		dst = idst.data();
		stridedst = idst.stride();
		src1 = dx.data();
		stridesrc1 = dx.stride();
		src2 = dy.data();
		stridesrc2 = dy.stride();

		h = idst.height();
		while( h-- ) {
			pdst = ( float* ) dst;
			psrc1 = ( float* ) src1;
			psrc2 = ( float* ) src2;

			w = idst.width() * idst.channels();
			while( w-- ) {
				*pdst++ += lambda * ( *psrc1++ + *psrc2++ );
			}

			dst += stridedst;
			src1 += stridesrc1;
			src2 += stridesrc2;
		}
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

		dst = idst.data();
		stridedst = idst.stride();
		src1 = isrc.data();
		stridesrc1 = isrc.stride();
		src2 = dx.data();
		stridesrc2 = dx.stride();
		src3 = dy.data();
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
	}

	static void multadd2_th( Image& idst1, Image& idst2, Image& dx, Image& dy, float taulambda )
	{
		uint8_t* dst1;
		uint8_t* dst2;
		uint8_t* src1;
		uint8_t* src2;
		size_t stridedst1;
		size_t stridedst2;
		size_t stridesrc1;
		size_t stridesrc2;
		float* pdst1;
		float* pdst2;
		float* psrc1;
		float* psrc2;
		size_t w, h;
		float tmp1, tmp2, norm;

		dst1 = idst1.data();
		stridedst1 = idst1.stride();
		dst2 = idst2.data();
		stridedst2 = idst2.stride();
		src1 = dx.data();
		stridesrc1 = dx.stride();
		src2 = dy.data();
		stridesrc2 = dy.stride();


		h = idst1.height();
		while( h-- ) {
			pdst1 = ( float* ) dst1;
			pdst2 = ( float* ) dst2;
			psrc1 = ( float* ) src1;
			psrc2 = ( float* ) src2;

			w = idst1.width() * idst1.channels();
			while( w-- ) {
				tmp1 = *pdst1 + taulambda * *psrc1++;
				tmp2 = *pdst2 + taulambda * *psrc2++;
				norm = 1.0f / Math::max( 1.0f, Math::sqrt( tmp1 * tmp1 + tmp2 * tmp2 ) );
				*pdst1++ = tmp1 * norm;
				*pdst2++ = tmp2 * norm;

//				norm = 1.0f / Math::max( 1.0f,  Math::abs( tmp1 ) + Math::abs( tmp2 ) );
//				*pdst1++ = tmp1 / Math::max( 1.0f, Math::abs( tmp1 ) );
//				*pdst2++ = tmp2 / Math::max( 1.0f, Math::abs( tmp2 ) );
			}

			dst1 += stridedst1;
			dst2 += stridedst2;
			src1 += stridesrc1;
			src2 += stridesrc2;
		}
	}

	void ROFDenoise::apply( Image& dst, const Image& src, float lambda, size_t iter )
	{
		Image dx, dy, px, py;
#if 0
		Image kerndx( 5, 1, CVT_GRAY, CVT_FLOAT );
		Image kerndy( 1, 5, CVT_GRAY, CVT_FLOAT );
		Image kerndxrev( 5, 1, CVT_GRAY, CVT_FLOAT );
		Image kerndyrev( 1, 5, CVT_GRAY, CVT_FLOAT );

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
		Image kerndx( 3, 1, CVT_GRAY, CVT_FLOAT );
		Image kerndy( 1, 3, CVT_GRAY, CVT_FLOAT );
		Image kerndxrev( 3, 1, CVT_GRAY, CVT_FLOAT );
		Image kerndyrev( 1, 3, CVT_GRAY, CVT_FLOAT );

		{
			float* data;
			data = ( float* ) kerndx.data();
			*data++ = -1.0f;
			*data++ =  1.0f;
			*data++ =  0.0f;

			data = ( float* ) kerndy.scanline( 0 );
			*data++ = -1.0f;
			data = ( float* ) kerndy.scanline( 1 );
			*data++ =  1.0f;
			data = ( float* ) kerndy.scanline( 2 );
			*data++ =  0.0f;

			data = ( float* ) kerndxrev.data();
			*data++ =  0.0f;
			*data++ = -1.0f;
			*data++ =  1.0f;

			data = ( float* ) kerndyrev.scanline( 0 );
			*data++ =  0.0f;
			data = ( float* ) kerndyrev.scanline( 1 );
			*data++ = -1.0f;
			data = ( float* ) kerndyrev.scanline( 2 );
			*data++ =  1.0f;
		}
#endif

#define TAU 0.249f
		dst.copy( src );
		px.reallocate( src );
		py.reallocate( src );
		px.fill( Color( 0.0f, 0.0f ) );
		py.fill( Color( 0.0f, 0.0f ) );

		while( iter-- ) {
			dst.convolve( dx, kerndx, false );
			dst.convolve( dy, kerndy, false );
			multadd2_th( px, py, dx, dy, TAU / lambda );
			px.convolve( dx, kerndxrev, false );
			py.convolve( dy, kerndyrev, false );
			multadd3( dst, src, dx, dy, lambda );
		}

	}

}
