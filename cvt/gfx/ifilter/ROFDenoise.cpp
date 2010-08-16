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

	    w = idst.width();
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

	    w = idst1.width();
	    while( w-- ) {
		tmp1 = *pdst1 + taulambda * *psrc1++;
		tmp2 = *pdst2 + taulambda * *psrc2++;
		norm = 1.0f / Math::max( 1.0f, sqrtf( tmp1 * tmp1 + tmp2 * tmp2 ) );
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

	dst.copy( src );
	src.ddx( px );
	src.ddy( py );

#define TAU 0.25f

	while( iter-- ) {
	    px.ddx( dx );
	    py.ddy( dy );
	    ptmp =  src;
	    multadd2( ptmp, dx, dy, lambda );
	    ptmp.ddx( dx, false );
	    ptmp.ddy( dy, false );
	    multadd2_th( px, py, dx, dy, TAU / lambda );
	}
	px.ddx( dx );
	py.ddy( dy );
	multadd2( dst, dx, dy, lambda );
    }

}
