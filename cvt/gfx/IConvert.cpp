#include <cvt/gfx/IConvert.h>
#include <cvt/util/SIMD.h>

namespace cvt {

#define LAST_FORMAT	( IFORMAT_UYVY_UINT8 )

#define TABLE( table, source, dst ) table[ ( ( source ) - 1 ) * LAST_FORMAT + ( dst ) - 1 ]

	IConvert * IConvert::_instance = 0;


	#define CONV( func, dI, dsttype, sI, srctype, width )				\
	{																	\
		sbase = src = sI.map( &sstride );								\
		dbase = dst = dI.map( &dstride );								\
		h = sI.height();												\
		while( h-- ) {													\
			simd->func( ( dsttype ) dst, ( const srctype ) src, width );\
			src += sstride;												\
			dst += dstride;												\
		}																\
		sI.unmap( sbase );													\
		dI.unmap( dbase );												\
		return;															\
	}

	static void Conv_XYZAf_to_ZYXAf( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;
		CONV( Conv_XYZAf_to_ZYXAf, dstImage, float*, sourceImage, float*, sourceImage.width() )
	}

	static void Conv_XYZAu8_to_ZYXAu8( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;
		CONV( Conv_XYZAu8_to_ZYXAu8, dstImage, uint8_t*, sourceImage, uint8_t*, sourceImage.width() )
	}

	static void Conv_u8_to_f( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;
		CONV( Conv_u8_to_f, dstImage, float*, sourceImage, uint8_t*, sourceImage.width() * dstImage.channels() )
	}

	static void Conv_f_to_u8( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_f_to_u8, dstImage, uint8_t*, sourceImage, float*, sourceImage.width() * dstImage.channels() )
	}

	static void Conv_RGBAu8_to_GRAYf( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_RGBAu8_to_GRAYf, dstImage, float*, sourceImage, uint8_t*, sourceImage.width() )
	}

	static void Conv_GRAYu8_to_XXXAu8( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_GRAYu8_to_XXXAu8, dstImage, uint8_t*, sourceImage, uint8_t*, sourceImage.width() )
	}

	static void Conv_XXXAu8_to_XXXAf( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_XXXAu8_to_XXXAf, dstImage, float*, sourceImage, uint8_t*, sourceImage.width() )
	}

	static void Conv_XXXAf_to_XXXAu8( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_XXXAf_to_XXXAu8, dstImage, uint8_t*, sourceImage, float*, sourceImage.width() )
	}

	static void Conv_XYZAu8_to_ZYXAf( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_XYZAu8_to_ZYXAf, dstImage, float*, sourceImage, uint8_t*, sourceImage.width() )
	}

	static void Conv_XYZAf_to_ZYXAu8( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_XYZAf_to_ZYXAu8, dstImage, uint8_t*, sourceImage, float*, sourceImage.width() )
	}

	static void Conv_BGRAu8_to_GRAYu8( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_BGRAu8_to_GRAYu8, dstImage, uint8_t*, sourceImage, uint8_t*, sourceImage.width() )
	}

	static void Conv_RGBAu8_to_GRAYu8( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_RGBAu8_to_GRAYu8, dstImage, uint8_t*, sourceImage, uint8_t*, sourceImage.width() )
	}


	static void Conv_BGRAu8_to_GRAYf( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_BGRAu8_to_GRAYf, dstImage, float*, sourceImage, uint8_t*, sourceImage.width() )
	}

	static void Conv_YUYVu8_to_RGBAu8( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_YUYVu8_to_RGBAu8, dstImage, uint8_t*, sourceImage, uint8_t*, sourceImage.width() )
	}

	static void Conv_YUYVu8_to_BGRAu8( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_YUYVu8_to_BGRAu8, dstImage, uint8_t*, sourceImage, uint8_t*, sourceImage.width() )
	}

	static void Conv_UYVYu8_to_RGBAu8( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_UYVYu8_to_RGBAu8, dstImage, uint8_t*, sourceImage, uint8_t*, sourceImage.width() )
	}

	static void Conv_UYVYu8_to_BGRAu8( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::instance();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_UYVYu8_to_BGRAu8, dstImage, uint8_t*, sourceImage, uint8_t*, sourceImage.width() )
	}
#undef CONV

	void _debayer_RGGB_to_RGBAu8( Image & dstImage, const Image & sourceImage )
	{
		const uint32_t* src1;
		const uint32_t* src2;
		const uint32_t* src3;
		const uint8_t* osrc;
		uint32_t* dst;
		uint8_t* odst;
		size_t sstride;
		size_t dstride;
		size_t h;
		size_t w;

		SIMD* simd = SIMD::instance();

		osrc = sourceImage.map( &sstride );
		src1 = ( uint32_t* ) osrc;
		sstride = sstride >> 2;
		src2 = src1 + sstride;
		src3 = src2 + sstride;
		w = sourceImage.width();
		h = sourceImage.height();
		odst = dstImage.map( &dstride );
		dst = ( uint32_t* ) odst;
		dstride = dstride >> 2;

		simd->debayer_ODD_RGGBu8_RGBAu8( dst, src2, src1, src2, w >> 2 );
		dst += dstride;
		h = h - 2 ;
		while( h-- ) {
			if(  h & 1 ) {
				simd->debayer_EVEN_RGGBu8_RGBAu8( dst, src1, src2, src3, w >> 2 );
			} else {
				simd->debayer_ODD_RGGBu8_RGBAu8( dst, src1, src2, src3, w >> 2 );
			}
			dst += dstride;
			src1 += sstride;
			src2 += sstride;
			src3 += sstride;
		}
		simd->debayer_ODD_RGGBu8_RGBAu8( dst, src2, src1, src2, w >> 2 );
		dstImage.unmap( odst );
		sourceImage.unmap( osrc );
	}

	void _debayer_RGGB_to_BGRAu8( Image & dstImage, const Image & sourceImage )
	{
		const uint32_t* src1;
		const uint32_t* src2;
		const uint32_t* src3;
		const uint8_t* osrc;
		uint32_t* dst;
		uint8_t* odst;
		size_t sstride;
		size_t dstride;
		size_t h;
		size_t w;

		SIMD* simd = SIMD::instance();

		osrc = sourceImage.map( &sstride );
		src1 = ( uint32_t* ) osrc;
		sstride = sstride >> 2;
		src2 = src1 + sstride;
		src3 = src2 + sstride;
		w = sourceImage.width();
		h = sourceImage.height();
		odst = dstImage.map( &dstride );
		dst = ( uint32_t* ) odst;
		dstride = dstride >> 2;

		simd->debayer_ODD_RGGBu8_BGRAu8( dst, src2, src1, src2, w >> 2 );
		dst += dstride;
		h = h - 2 ;
		while( h-- ) {
			if(  h & 1 ) {
				simd->debayer_EVEN_RGGBu8_BGRAu8( dst, src1, src2, src3, w >> 2 );
			} else {
				simd->debayer_ODD_RGGBu8_BGRAu8( dst, src1, src2, src3, w >> 2 );
			}
			dst += dstride;
			src1 += sstride;
			src2 += sstride;
			src3 += sstride;
		}
		simd->debayer_ODD_RGGBu8_BGRAu8( dst, src2, src1, src2, w >> 2 );
		dstImage.unmap( odst );
		sourceImage.unmap( osrc );
	}

	void _debayer_RGGB_to_GRAYu8( Image & dstImage, const Image & sourceImage )
	{
		const uint32_t* src1;
		const uint32_t* src2;
		const uint32_t* src3;
		const uint8_t* osrc;
		uint32_t* dst;
		uint8_t* odst;
		size_t sstride;
		size_t dstride;
		size_t h;
		size_t w;

		SIMD* simd = SIMD::instance();

		osrc = sourceImage.map( &sstride );
		src1 = ( uint32_t* ) osrc;
		sstride = sstride >> 2;
		src2 = src1 + sstride;
		src3 = src2 + sstride;
		w = sourceImage.width();
		h = sourceImage.height();
		odst = dstImage.map( &dstride );
		dst = ( uint32_t* ) odst;
		dstride = dstride >> 2;

		simd->debayer_ODD_RGGBu8_GRAYu8( dst, src2, src1, src2, w >> 2 );
		dst += dstride;
		h = h - 2 ;
		while( h-- ) {
			if(  h & 1 ) {
				simd->debayer_EVEN_RGGBu8_GRAYu8( dst, src1, src2, src3, w >> 2 );
			} else {
				simd->debayer_ODD_RGGBu8_GRAYu8( dst, src1, src2, src3, w >> 2 );
			}
			dst += dstride;
			src1 += sstride;
			src2 += sstride;
			src3 += sstride;
		}
		simd->debayer_ODD_RGGBu8_GRAYu8( dst, src2, src1, src2, w >> 2 );
		dstImage.unmap( odst );
		sourceImage.unmap( osrc );
	}

	static inline void _pixelYUYV2BGRA( uint32_t yuyv, uint32_t& p1, uint32_t& p2 )
	{
		int r, g, b;
		int y0, y1, u, v;

		v = ( yuyv >> 24 ) - 128;
		y1 = ( ( yuyv >> 16 ) & 0xff );
		u = ( ( yuyv >> 8 ) & 0xff ) - 128;
		y0 = ( ( yuyv ) & 0xff );
		r = ((v*1436) >> 10);
		g = ((u*352 + v*731) >> 10);
		b = ((u*1814) >> 10);

		// clamp the values
		p1 = 0xff000000;
		p1 |= Math::clamp( y0 + r, 0, 255 ) << 16;
		p1 |= Math::clamp( y0 - g, 0, 255 ) << 8;
		p1 |= Math::clamp( y0 + b, 0, 255 );
		p2 = 0xff000000;
		p2 |= Math::clamp( y1 + r, 0, 255 ) << 16;
		p2 |= Math::clamp( y1 - g, 0, 255 ) << 8;
		p2 |= Math::clamp( y1 + b, 0, 255 );
	}

	static inline void _pixelUYVY2BGRA( uint32_t uyvy, uint32_t& p1, uint32_t& p2 )
	{
		int r, g, b;
		int y0, y1, u, v;

		v = ( ( uyvy >> 16 ) & 0xff ) - 128;
		y1 = ( uyvy >> 24 );
		u = ( uyvy & 0xff ) - 128;
		y0 = ( ( uyvy >> 8 ) & 0xff );
		r = ((v*1436) >> 10);
		g = ((u*352 + v*731) >> 10);
		b = ((u*1814) >> 10);

		// clamp the values
		p1 = 0xff000000;
		p1 |= Math::clamp( y0 + r, 0, 255 ) << 16;
		p1 |= Math::clamp( y0 - g, 0, 255 ) << 8;
		p1 |= Math::clamp( y0 + b, 0, 255 );
		p2 = 0xff000000;
		p2 |= Math::clamp( y1 + r, 0, 255 ) << 16;
		p2 |= Math::clamp( y1 - g, 0, 255 ) << 8;
		p2 |= Math::clamp( y1 + b, 0, 255 );
	}

	static inline void _pixelUYVY2RGBA( uint32_t uyvy, uint32_t& p1, uint32_t& p2 )
	{
		int r, g, b;
		int y0, y1, u, v;

		v = ( ( uyvy >> 16 ) & 0xff ) - 128;
		y1 = ( uyvy >> 24 );
		u = ( uyvy & 0xff ) - 128;
		y0 = ( ( uyvy >> 8 ) & 0xff );
		r = ((v*1436) >> 10);
		g = ((u*352 + v*731) >> 10);
		b = ((u*1814) >> 10);

		// clamp the values
		p1 = 0xff000000;
		p1 |= Math::clamp( y0 + r, 0, 255 );
		p1 |= Math::clamp( y0 - g, 0, 255 ) << 8;
		p1 |= Math::clamp( y0 + b, 0, 255 ) << 16;
		p2 = 0xff000000;
		p2 |= Math::clamp( y1 + r, 0, 255 );
		p2 |= Math::clamp( y1 - g, 0, 255 ) << 8;
		p2 |= Math::clamp( y1 + b, 0, 255 ) << 16;
	}

	static inline void _pixelYUYV2RGBA( uint32_t yuyv, uint32_t& p1, uint32_t& p2 )
	{
		int r, g, b;
		int y0, y1, u, v;

		v = ( yuyv >> 24 ) - 128;
		y1 = ( ( yuyv >> 16 ) & 0xff );
		u = ( ( yuyv >> 8 ) & 0xff ) - 128;
		y0 = ( ( yuyv ) & 0xff );
		r = ((v*1436) >> 10);
		g = ((u*352 + v*731) >> 10);
		b = ((u*1814) >> 10);

		// clamp the values
		p1 = 0xff000000;
		p1 |= Math::clamp( y0 + r, 0, 255 );
		p1 |= Math::clamp( y0 - g, 0, 255 ) << 8;
		p1 |= Math::clamp( y0 + b, 0, 255 ) << 16;
		p2 = 0xff000000;
		p2 |= Math::clamp( y1 + r, 0, 255 );
		p2 |= Math::clamp( y1 - g, 0, 255 ) << 8;
		p2 |= Math::clamp( y1 + b, 0, 255 ) << 16;
	}


/*	static void Conv_YUYVu8_to_RGBAu8( Image & dstImage, const Image & sourceImage )
	{
		size_t l = dstImage.height();
		size_t w = dstImage.width();
		size_t c;
		uint32_t *s;
		uint32_t *d;
		uint32_t p1, p2;

		size_t stridesrc;
		size_t stridedst;
		const uint8_t * sOrig;
	    uint8_t * dOrig;
		const uint8_t * src = sourceImage.map( &stridesrc );
		uint8_t * dst = dstImage.map( &stridedst );
		sOrig = src;
		dOrig = dst;

		while( l-- ) {
			s = ( uint32_t* ) src;
			d = ( uint32_t* ) dst;
			c = w >> 1;
			while (c--) {
				_pixelYUYV2RGBA( *s++, p1, p2 );
				*d++ = p1;
				*d++ = p2;
			}
			src += stridesrc;
			dst += stridedst;
		}
		sourceImage.unmap( sOrig );
		dstImage.unmap( dOrig );
	}

	static void Conv_YUYVu8_to_BGRAu8( Image & dstImage, const Image & sourceImage )
	{
		size_t l = dstImage.height();
		size_t w = dstImage.width();
		size_t c;
		uint32_t *s;
		uint32_t *d;
		uint32_t p1, p2;

		size_t stridesrc;
		size_t stridedst;
		const uint8_t * sOrig;
	    uint8_t * dOrig;
		const uint8_t * src = sourceImage.map( &stridesrc );
		uint8_t * dst = dstImage.map( &stridedst );
		sOrig = src;
		dOrig = dst;


		while( l-- ) {
			s = ( uint32_t* ) src;
			d = ( uint32_t* ) dst;
			c = w >> 1;
			while (c--) {
				_pixelYUYV2BGRA( *s++, p1, p2 );
				*d++ = p1;
				*d++ = p2;
			}
			src += stridesrc;
			dst += stridedst;
		}
		sourceImage.unmap( sOrig );
		dstImage.unmap( dOrig );
	}
	*/

	static void Conv_YUYVu8_to_GRAYu8( Image & dstImage, const Image & sourceImage )
	{
		size_t l = dstImage.height();
		size_t w = dstImage.width();
		size_t c;
		uint32_t *s;
		uint8_t *d;
		int tmp;

		size_t stridesrc;
		size_t stridedst;
		const uint8_t * sOrig;
	    uint8_t * dOrig;
		const uint8_t * src = sourceImage.map( &stridesrc );
		uint8_t * dst = dstImage.map( &stridedst );
		sOrig = src;
		dOrig = dst;

		while( l-- ) {
			s = ( uint32_t* ) src;
			d = dst;
			c = w >> 1;
			while (c--) {
				tmp = ( ( ( int ) ( *s & 0xff ) - 16 ) * 1192 ) >> 10;
				*d++ = Math::clamp( tmp, 0, 255 );
				tmp = ( ( ( ( int ) ( ( *s++ ) >> 16 ) & 0xff ) - 16 ) * 1192 ) >> 10;
				*d++ = Math::clamp( tmp, 0, 255 );
			}
			src += stridesrc;
			dst += stridedst;
		}

		sourceImage.unmap( sOrig );
		dstImage.unmap( dOrig );
	}

	static void Conv_YUYVu8_to_GRAYALPHAu8( Image & dstImage, const Image & sourceImage )
	{
		size_t l = dstImage.height();
		size_t w = dstImage.width();
		size_t c;
		uint32_t *s;
		uint8_t *d;
		int tmp;


		size_t stridesrc;
		size_t stridedst;
		const uint8_t * sOrig;
	    uint8_t * dOrig;
		const uint8_t * src = sourceImage.map( &stridesrc );
		uint8_t * dst = dstImage.map( &stridedst );
		sOrig = src;
		dOrig = dst;

		while( l-- ) {
			s = ( uint32_t* ) src;
			d = dst;
			c = w >> 1;
			while (c--) {
				tmp = ( ( ( int ) ( *s & 0xff ) - 16 ) * 1192 ) >> 10;
				*d++ = Math::clamp( tmp, 0, 255 );
				*d++ = 255;
				tmp = ( ( ( int ) ( ( ( *s++ ) >> 16 ) & 0xff ) - 16 ) * 1192 ) >> 10;
				*d++ = Math::clamp( tmp, 0, 255 );
				*d++ = 255;
			}
			src += stridesrc;
			dst += stridedst;
		}

		sourceImage.unmap( sOrig );
		dstImage.unmap( dOrig );
	}

/*	static void Conv_UYVYu8_to_RGBAu8( Image & dstImage, const Image & sourceImage )
	{
		size_t l = dstImage.height();
		size_t w = dstImage.width();
		size_t c;
		uint32_t *s;
		uint32_t *d;
		uint32_t p1, p2;

		size_t stridesrc;
		size_t stridedst;
		const uint8_t * sOrig;
	    uint8_t * dOrig;
		const uint8_t * src = sourceImage.map( &stridesrc );
		uint8_t * dst = dstImage.map( &stridedst );
		sOrig = src;
		dOrig = dst;

		while( l-- ) {
			s = ( uint32_t* ) src;
			d = ( uint32_t* ) dst;
			c = w >> 1;
			while (c--) {
				_pixelUYVY2RGBA( *s++, p1, p2 );
				*d++ = p1;
				*d++ = p2;
			}
			src += stridesrc;
			dst += stridedst;
		}
		sourceImage.unmap( sOrig );
		dstImage.unmap( dOrig );
	}

	static void Conv_UYVYu8_to_BGRAu8( Image & dstImage, const Image & sourceImage )
	{
		size_t l = dstImage.height();
		size_t w = dstImage.width();
		size_t c;
		uint32_t *s;
		uint32_t *d;
		uint32_t p1, p2;

		size_t stridesrc;
		size_t stridedst;
		const uint8_t * sOrig;
	    uint8_t * dOrig;
		const uint8_t * src = sourceImage.map( &stridesrc );
		uint8_t * dst = dstImage.map( &stridedst );
		sOrig = src;
		dOrig = dst;


		while( l-- ) {
			s = ( uint32_t* ) src;
			d = ( uint32_t* ) dst;
			c = w >> 1;
			while (c--) {
				_pixelUYVY2BGRA( *s++, p1, p2 );
				*d++ = p1;
				*d++ = p2;
			}
			src += stridesrc;
			dst += stridedst;
		}
		sourceImage.unmap( sOrig );
		dstImage.unmap( dOrig );
	}*/

	static void Conv_UYVYu8_to_GRAYu8( Image & dstImage, const Image & sourceImage )
	{
		size_t l = dstImage.height();
		size_t w = dstImage.width();
		size_t c;
		uint32_t *s;
		uint8_t *d;
		int tmp;

		size_t stridesrc;
		size_t stridedst;
		const uint8_t * sOrig;
	    uint8_t * dOrig;
		const uint8_t * src = sourceImage.map( &stridesrc );
		uint8_t * dst = dstImage.map( &stridedst );
		sOrig = src;
		dOrig = dst;

		while( l-- ) {
			s = ( uint32_t* ) src;
			d = dst;
			c = w >> 1;
			while (c--) {
				tmp = ( ( ( int ) ( ( ( *s ) >> 8 ) & 0xff ) - 16 ) * 1192 ) >> 10;
				*d++ = Math::clamp( tmp, 0, 255 );
				tmp = ( ( ( int ) ( ( ( *s++ ) >> 24 ) & 0xff ) - 16 ) * 1192 ) >> 10;
				*d++ = Math::clamp( tmp, 0, 255 );
			}
			src += stridesrc;
			dst += stridedst;
		}

		sourceImage.unmap( sOrig );
		dstImage.unmap( dOrig );
	}

	static void Conv_UYVYu8_to_GRAYALPHAu8( Image & dstImage, const Image & sourceImage )
	{
		size_t l = dstImage.height();
		size_t w = dstImage.width();
		size_t c;
		uint32_t *s;
		uint8_t *d;
		int tmp;

		size_t stridesrc;
		size_t stridedst;
		const uint8_t * sOrig;
	    uint8_t * dOrig;
		const uint8_t * src = sourceImage.map( &stridesrc );
		uint8_t * dst = dstImage.map( &stridedst );
		sOrig = src;
		dOrig = dst;

		while( l-- ) {
			s = ( uint32_t* ) src;
			d = dst;
			c = w >> 1;
			while (c--) {
				tmp = ( ( ( int ) ( ( ( *s ) >> 8 ) & 0xff ) - 16 ) * 1192 ) >> 10;
				*d++ = Math::clamp( tmp, 0, 255 );
				*d++ = 255;
				tmp = ( ( ( int ) ( ( ( *s++ ) >> 24 ) & 0xff ) - 16 ) * 1192 ) >> 10;
				*d++ = Math::clamp( tmp, 0, 255 );
				*d++ = 255;
			}
			src += stridesrc;
			dst += stridedst;
		}

		sourceImage.unmap( sOrig );
		dstImage.unmap( dOrig );
	}

	IConvert::IConvert():
		_convertFuncs( 0 )
	{
		_convertFuncs = new ConversionFunction[ Math::sqr( (int)LAST_FORMAT ) ];
		memset( _convertFuncs, 0, Math::sqr( (int)LAST_FORMAT ) );

		this->initTable();
	}

	void IConvert::initTable()
	{
		/* GRAY_UINT8 TO X */
		TABLE( _convertFuncs, IFORMAT_GRAY_UINT8, IFORMAT_GRAY_FLOAT ) = &Conv_u8_to_f;
		TABLE( _convertFuncs, IFORMAT_GRAY_UINT8, IFORMAT_BGRA_UINT8 ) = &Conv_GRAYu8_to_XXXAu8;
		TABLE( _convertFuncs, IFORMAT_GRAY_UINT8, IFORMAT_RGBA_UINT8 ) = &Conv_GRAYu8_to_XXXAu8;

		/* GRAY_FLOAT TO X */
		TABLE( _convertFuncs, IFORMAT_GRAY_FLOAT, IFORMAT_GRAY_UINT8 ) = &Conv_f_to_u8;

		/* GRAYALPHA_UINT8 TO X */
		TABLE( _convertFuncs, IFORMAT_GRAYALPHA_UINT8, IFORMAT_GRAYALPHA_FLOAT ) = &Conv_u8_to_f;

		/* GRAYALPHA_FLOAT TO X */
		TABLE( _convertFuncs, IFORMAT_GRAYALPHA_FLOAT, IFORMAT_GRAYALPHA_UINT8 ) = &Conv_f_to_u8;

		/* RGBA_UINT8 TO X */
		TABLE( _convertFuncs, IFORMAT_RGBA_UINT8, IFORMAT_GRAY_UINT8 ) = &Conv_RGBAu8_to_GRAYu8;
		TABLE( _convertFuncs, IFORMAT_RGBA_UINT8, IFORMAT_GRAY_FLOAT ) = &Conv_RGBAu8_to_GRAYf;
		TABLE( _convertFuncs, IFORMAT_RGBA_UINT8, IFORMAT_RGBA_FLOAT ) = &Conv_XXXAu8_to_XXXAf;
		TABLE( _convertFuncs, IFORMAT_RGBA_UINT8, IFORMAT_BGRA_UINT8 ) = &Conv_XYZAu8_to_ZYXAu8;
		TABLE( _convertFuncs, IFORMAT_RGBA_UINT8, IFORMAT_BGRA_FLOAT ) = &Conv_XYZAu8_to_ZYXAf;

		/* RGBA_FLOAT TO X */
		TABLE( _convertFuncs, IFORMAT_RGBA_FLOAT, IFORMAT_RGBA_UINT8 ) = &Conv_XXXAf_to_XXXAu8;
		TABLE( _convertFuncs, IFORMAT_RGBA_FLOAT, IFORMAT_BGRA_UINT8 ) = &Conv_XYZAf_to_ZYXAu8;
		TABLE( _convertFuncs, IFORMAT_RGBA_FLOAT, IFORMAT_BGRA_FLOAT ) = &Conv_XYZAf_to_ZYXAf;

		/* BGRA_UINT8 TO X */
		TABLE( _convertFuncs, IFORMAT_BGRA_UINT8, IFORMAT_GRAY_UINT8 ) = &Conv_BGRAu8_to_GRAYu8;
		TABLE( _convertFuncs, IFORMAT_BGRA_UINT8, IFORMAT_GRAY_FLOAT ) = &Conv_BGRAu8_to_GRAYf;
		TABLE( _convertFuncs, IFORMAT_BGRA_UINT8, IFORMAT_RGBA_UINT8 ) = &Conv_XYZAu8_to_ZYXAu8;
		TABLE( _convertFuncs, IFORMAT_BGRA_UINT8, IFORMAT_RGBA_FLOAT ) = &Conv_XYZAu8_to_ZYXAf;
		TABLE( _convertFuncs, IFORMAT_BGRA_UINT8, IFORMAT_BGRA_FLOAT ) = &Conv_XXXAu8_to_XXXAf;

		/* BGRA_FLOAT TO X */
		TABLE( _convertFuncs, IFORMAT_BGRA_FLOAT, IFORMAT_BGRA_UINT8 ) = &Conv_XXXAf_to_XXXAu8;
		TABLE( _convertFuncs, IFORMAT_BGRA_FLOAT, IFORMAT_RGBA_UINT8 ) = &Conv_XYZAf_to_ZYXAu8;
		TABLE( _convertFuncs, IFORMAT_BGRA_FLOAT, IFORMAT_RGBA_FLOAT ) = &Conv_XYZAf_to_ZYXAf;

		/* RGGB_UINT8 to X */
		TABLE( _convertFuncs, IFORMAT_BAYER_RGGB_UINT8, IFORMAT_GRAY_UINT8 ) = &_debayer_RGGB_to_GRAYu8;
		TABLE( _convertFuncs, IFORMAT_BAYER_RGGB_UINT8, IFORMAT_RGBA_UINT8 ) = &_debayer_RGGB_to_RGBAu8;
		TABLE( _convertFuncs, IFORMAT_BAYER_RGGB_UINT8, IFORMAT_BGRA_UINT8 ) = &_debayer_RGGB_to_BGRAu8;

		/* YUYV_UINT8 to X */
		TABLE( _convertFuncs, IFORMAT_YUYV_UINT8, IFORMAT_GRAY_UINT8 ) = &Conv_YUYVu8_to_GRAYu8;
		TABLE( _convertFuncs, IFORMAT_YUYV_UINT8, IFORMAT_GRAYALPHA_UINT8 ) = &Conv_YUYVu8_to_GRAYALPHAu8;
		TABLE( _convertFuncs, IFORMAT_YUYV_UINT8, IFORMAT_RGBA_UINT8 ) = &Conv_YUYVu8_to_RGBAu8;
		TABLE( _convertFuncs, IFORMAT_YUYV_UINT8, IFORMAT_BGRA_UINT8 ) = &Conv_YUYVu8_to_BGRAu8;

		/* UYVY_UINT8 to X */
		TABLE( _convertFuncs, IFORMAT_UYVY_UINT8, IFORMAT_GRAY_UINT8 ) = &Conv_UYVYu8_to_GRAYu8;
		TABLE( _convertFuncs, IFORMAT_UYVY_UINT8, IFORMAT_GRAYALPHA_UINT8 ) = &Conv_UYVYu8_to_GRAYALPHAu8;
		TABLE( _convertFuncs, IFORMAT_UYVY_UINT8, IFORMAT_RGBA_UINT8 ) = &Conv_UYVYu8_to_RGBAu8;
		TABLE( _convertFuncs, IFORMAT_UYVY_UINT8, IFORMAT_BGRA_UINT8 ) = &Conv_UYVYu8_to_BGRAu8;
	}

	const IConvert & IConvert::instance()
	{
		if( IConvert::_instance == 0 ){
			_instance = new IConvert();
		}

		return *_instance;
	}


	void IConvert::convert( Image & dst, const Image & src )
	{
		//checkSize( src, __PRETTY_FUNCTION__, __LINE__, _mem->_width, _mem->_height );
		if( src.format() == dst.format() ) {
			dst.copy( src );
			return;
		}

		IFormatID sourceID = src.format().formatID;
		IFormatID dstID = dst.format().formatID;

		if( sourceID > LAST_FORMAT )
			throw CVTException( "Source format unkown" );
		if( dstID > LAST_FORMAT )
			throw CVTException( "Destination format unkown" );

		IConvert self = IConvert::instance();
		if( self.TABLE( _convertFuncs, sourceID, dstID ) ){
			self.TABLE( _convertFuncs, sourceID, dstID)( dst, src );
		} else {
			std::cerr << "CONVERSION MISSING: " << src.format() << " -> " << dst.format() << std::endl;
			throw CVTException( "Conversion not implemented!" );
		}
	}

}
