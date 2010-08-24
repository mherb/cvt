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
			}

			dst1 += stridedst1;
			dst2 += stridedst2;
			src1 += stridesrc1;
			src2 += stridesrc2;
		}
	}

	void ROFDenoise::apply( Image& dst, const Image& src, float lambda, size_t iter )
	{
		Image dx, dy, px, py, ptmp;
		Image kerndx( 5, 1, CVT_GRAY, CVT_FLOAT );
		Image kerndy( 1, 5, CVT_GRAY, CVT_FLOAT );
		Image kerndxrev( 5, 1, CVT_GRAY, CVT_FLOAT );
		Image kerndyrev( 1, 5, CVT_GRAY, CVT_FLOAT );

		{
			float* data;
			data = ( float* ) kerndx.data();
			*data++ = -0.1f;
			*data++ = -0.9f;
			*data++ =  0.9f;
			*data++ =  0.1f;
			*data++ =  0.0f;

			data = ( float* ) kerndy.scanline( 0 );
			*data++ = -0.1f;
			data = ( float* ) kerndy.scanline( 1 );
			*data++ = -0.9f;
			data = ( float* ) kerndy.scanline( 2 );
			*data++ =  0.9f;
			data = ( float* ) kerndy.scanline( 3 );
			*data++ =  0.1f;
			data = ( float* ) kerndy.scanline( 4 );
			*data++ =  0.0f;

			data = ( float* ) kerndxrev.data();
			*data++ =  0.0f;
			*data++ = -0.1f;
			*data++ = -0.9f;
			*data++ =  0.9f;
			*data++ =  0.1f;

			data = ( float* ) kerndyrev.scanline( 0 );
			*data++ =  0.0f;
			data = ( float* ) kerndyrev.scanline( 1 );
			*data++ = -0.1f;
			data = ( float* ) kerndyrev.scanline( 2 );
			*data++ = -0.9f;
			data = ( float* ) kerndyrev.scanline( 3 );
			*data++ =  0.9f;
			data = ( float* ) kerndyrev.scanline( 4 );
			*data++ =  0.1f;




		}

#define TAU 0.25f

		dst.copy( src );
		src.convolve( dx, kerndx, false );
		src.convolve( dy, kerndy, false );
		px.reallocate( dx );
		py.reallocate( dy );
		px.fill( Color( 0.0f, 0.0f ) );
		py.fill( Color( 0.0f, 0.0f ) );

		while( iter-- ) {
			multadd2_th( px, py, dx, dy, TAU / lambda );
			px.convolve( dx, kerndx, false );
			py.convolve( dy, kerndy, false );
			ptmp = src;
			multadd2( ptmp, dx, dy, lambda );
			ptmp.convolve( dx, kerndxrev, false );
			ptmp.convolve( dy, kerndyrev, false );
		}
		px.convolve( dx, kerndx, false );
		py.convolve( dy, kerndy, false );
		multadd2( dst, dx, dy, lambda );
	}

}
