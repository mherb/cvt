#include <cvt/gfx/IConvert.h>
#include <cvt/util/SIMD.h>

namespace cvt {
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
		SIMD* simd = SIMD::get();
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
		SIMD* simd = SIMD::get();
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
		SIMD* simd = SIMD::get();
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
		SIMD* simd = SIMD::get();
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
		SIMD* simd = SIMD::get();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_RGBAu8_to_GRAYf, dstImage, float*, sourceImage, uint8_t*, sourceImage.width() )	
	}
	
	static void Conv_XXXAu8_to_XXXAf( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::get();
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
		SIMD* simd = SIMD::get();
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
		SIMD* simd = SIMD::get();
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
		SIMD* simd = SIMD::get();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;
		
		CONV( Conv_XYZAf_to_ZYXAu8, dstImage, uint8_t*, sourceImage, float*, sourceImage.width() )	
	}
	
	static void Conv_BGRAu8_to_GRAYf( Image & dstImage, const Image & sourceImage )
	{
		SIMD* simd = SIMD::get();
		const uint8_t* src;
		const uint8_t* sbase;
		size_t sstride;
		size_t dstride;
		uint8_t* dst;
		uint8_t* dbase;
		size_t h;

		CONV( Conv_BGRAu8_to_GRAYf, dstImage, float*, sourceImage, uint8_t*, sourceImage.width() )	
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
		size_t w, i;

		SIMD* simd = SIMD::get();
					
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
			i = ( w >> 2 ) - 2;
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
		size_t w, i;
		
		SIMD* simd = SIMD::get();
		
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
			i = ( w >> 2 ) - 2;
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
		size_t w, i;
		
		SIMD* simd = SIMD::get();
		
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
			i = ( w >> 2 ) - 2;
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
	
	/* source2dst table */
	static ConversionFunction _convertFuncs[ 17 ][ 17 ] = {
	/* G_U8     */	{ 0, 0, 0, &Conv_u8_to_f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	/* G_U16    */	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	/* G_I16    */	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	/* G_F	    */	{ &Conv_f_to_u8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	/* GA_U8    */	{ 0, 0, 0, 0, 0, 0, 0, &Conv_u8_to_f, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	/* GA_U16   */	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	/* GA_I16   */	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	/* GA_F     */	{ 0, 0, 0, 0, 0, 0, 0, &Conv_f_to_u8, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	/* RGBA_U8  */	{ 0, 0, 0, &Conv_RGBAu8_to_GRAYf, 0, 0, 0, 0, 0, 0, 0, &Conv_XXXAu8_to_XXXAf, &Conv_XYZAu8_to_ZYXAu8, 0, 0, &Conv_XYZAu8_to_ZYXAf, 0 },
	/* RGBA_U16 */	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	/* RGBA_I16 */	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	/* RGBA_F   */	{ 0, 0, 0, 0, 0, 0, 0, 0, &Conv_XXXAf_to_XXXAu8, 0, 0, 0, &Conv_XYZAf_to_ZYXAu8, 0, 0, &Conv_XYZAf_to_ZYXAf, 0 },
	/* BGRA_U8  */	{ 0, 0, 0, &Conv_BGRAu8_to_GRAYf, 0, 0, 0, 0, &Conv_XYZAu8_to_ZYXAu8, 0, 0, &Conv_XYZAu8_to_ZYXAf, 0, 0, 0, &Conv_XXXAu8_to_XXXAf, 0 },
	/* BGRA_U16 */	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	/* BGRA_I16 */	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	/* BGRA_F   */	{ 0, 0, 0, 0, 0, 0, 0, 0, &Conv_XYZAf_to_ZYXAu8, 0, 0, &Conv_XYZAf_to_ZYXAf, &Conv_XXXAf_to_XXXAu8, 0, 0, 0, 0 },
	/* RGGB_U8  */	{ &_debayer_RGGB_to_GRAYu8, 0, 0, 0, 0, 0, 0, 0, &_debayer_RGGB_to_RGBAu8, 0, 0, 0, &_debayer_RGGB_to_BGRAu8, 0, 0, 0, 0 }
	};
	
	void IConvert::convert( Image & dst, const Image & src )
	{	
		//checkSize( src, __PRETTY_FUNCTION__, __LINE__, _mem->_width, _mem->_height );
		if( src.format() == dst.format() ) {
			dst.copy( src );
			return;
		}
		
		IFormatID sourceID = src.format().formatID;
		IFormatID dstID = dst.format().formatID;
		
		if( sourceID - 1 > 17 )
			throw CVTException( "Source format unkown" );
		if( dstID - 1 > 17 )
			throw CVTException( "Destination format unkown" );
		
		if( _convertFuncs[ sourceID - 1 ][ dstID - 1 ] ){
			_convertFuncs[ sourceID - 1 ][ dstID - 1 ]( dst, src );
		} else {
			throw CVTException( "Conversion not implemented!" );
		}		
	}
	
}