#include "gfx/ifilter/ROFDenoise.h"
#include "gfx/IFilterScalar.h"
#include "gfx/IFilterParameterInfo.h"
#include "gfx/IFilterParameterSet.h"
#include "util/Exception.h"
#include "math/Math.h"

#include <iostream>

namespace cvt {

	static const IFilterParameterInfo _params[ 4 ] = {
		IFilterParameterInfo( "Input", IFILTERPARAMETER_IMAGE ),
		IFilterParameterInfo( "Output", IFILTERPARAMETER_IMAGE, IFILTERPARAMETER_OUT ),
		IFilterParameterInfo( "Iterations", IFILTERPARAMETER_SCALAR ),
		IFilterParameterInfo( "Lambda", IFILTERPARAMETER_SCALAR )
	};

	ROFDenoise::ROFDenoise() : IFilter( "ROFDenoise", _params, 4, IFILTER_CPU )
	{
	}

#if 0
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
#endif

	static void multadd3( Image& idst, const Image& isrc, const Image& dx, const Image& dy, float lambda )
	{
		uint8_t* dst;
		const uint8_t* src1;
		const uint8_t* src2;
		const uint8_t* src3;
		size_t stridedst;
		size_t stridesrc1;
		size_t stridesrc2;
		size_t stridesrc3;
		float* pdst;
		const float* psrc1;
		const float* psrc2;
		const float* psrc3;
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
		dy.unmap();
		dx.unmap();
		isrc.unmap();
		idst.unmap();
	}

	static void multadd2_th( Image& idst1, Image& idst2, const Image& dx, const Image& dy, float taulambda )
	{
		uint8_t* dst1;
		uint8_t* dst2;
		const uint8_t* src1;
		const uint8_t* src2;
		size_t stridedst1;
		size_t stridedst2;
		size_t stridesrc1;
		size_t stridesrc2;
		float* pdst1;
		float* pdst2;
		const float* psrc1;
		const float* psrc2;
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
				norm = Math::min( 1.0f, Math::fastInvSqrtf( tmp1 * tmp1 + tmp2 * tmp2 ) );
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
		dy.unmap();
		dx.unmap();
		idst2.unmap();
		idst1.unmap();
	}

	void ROFDenoise::apply( Image& dst, const Image& src, float lambda, size_t iter ) const
	{
		Image dx, dy, px, py;
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

#define TAU 0.249f
		dst.copy( src );
		px.reallocate( src );
		py.reallocate( src );
		px.fill( Color( 0.0f, 0.0f ) );
		py.fill( Color( 0.0f, 0.0f ) );

		while( iter-- ) {
			dst.convolve( dx, kerndx, false );
			dst.convolve( dy, kerndy, false );
//			dst.ddx( dx );
//			dst.ddy( dy );
			multadd2_th( px, py, dx, dy, TAU / lambda );
			px.convolve( dx, kerndxrev, false );
			py.convolve( dy, kerndyrev, false );
//			px.ddx( dx, false );
//			py.ddy( dy, false );

			multadd3( dst, src, dx, dy, lambda );
		}

	}

	void ROFDenoise::apply( const IFilterParameterSet* set, IFilterType t ) const
	{
		if( !(getIFilterType() & t) )
			throw CVTException("Invalid filter type (CPU/GPU)!");
		Image* dst;
		Image* src;
		IFilterScalar* lambda;
		IFilterScalar* iter;

		if( !set->isValid())
			throw CVTException("Invalid argument(s)");

		if( !( dst = dynamic_cast<Image*>( set->getParameter( 1 ) ) ) )
			throw CVTException("Invalid argument");
		if( !( src = dynamic_cast<Image*>( set->getParameter( 0 ) ) ) )
			throw CVTException("Invalid argument");
		if( !( lambda = dynamic_cast<IFilterScalar*>( set->getParameter( 3 ) ) ) )
			throw CVTException("Invalid argument");
		if( !( iter = dynamic_cast<IFilterScalar*>( set->getParameter( 2 ) ) ) )
			throw CVTException("Invalid argument");
		apply( *dst, *src, lambda->get(), ( size_t ) iter->get() );
	}

}
