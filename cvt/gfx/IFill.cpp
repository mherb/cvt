
#include <cvt/gfx/IFill.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/SIMD.h>

namespace cvt {

	template<typename TYPE, typename FILLTYPE>
	static void fillTemplate( Image& dst, FILLTYPE fill, void ( SIMD::*func )( TYPE*, FILLTYPE, size_t ) const )
	{
		IMapScoped<TYPE> map( dst );
		SIMD* simd = SIMD::instance();
		size_t w = dst.width();
		size_t h = dst.height();
		while( h-- ) {
			(simd->*func)( map.ptr(), fill, w );
			map++;
		}
	}

	void IFill::fill( Image& dst, const Color& c )
	{
		switch( dst.format().formatID )
		{
			case IFORMAT_GRAY_UINT8:
				{
					uint8_t v = ( uint8_t ) ( 255.0f * c.gray() );
					fillTemplate<uint8_t, uint8_t>( dst, v, &SIMD::SetValueU8 );
				}
				break;
			case IFORMAT_GRAY_FLOAT:
				{
					float v = c.gray();
					fillTemplate<float, float>( dst, v, &SIMD::SetValue1f );
				}
				break;
			case IFORMAT_GRAYALPHA_UINT8:
				{
					uint16_t v = ( uint16_t ) ( ( ( uint16_t ) ( 255.0f * c.alpha() ) ) << 8 );
					v |= ( ( uint16_t ) ( 255.0f * c.gray() ));
					fillTemplate<uint16_t, uint16_t>( dst, v, &SIMD::SetValueU16 );
				}
				break;
			case IFORMAT_GRAYALPHA_FLOAT:
				{
					float v[ 2 ] = { c.gray(), c.alpha() };
					fillTemplate<float, const float ( & ) [ 2 ]>( dst, v, &SIMD::SetValue2f );
				}
				break;
			case IFORMAT_RGBA_UINT8:
				{
					uint32_t v = ( ( uint32_t ) ( 255.0f * c.alpha() ) ) << 24;
					v |= ( ( uint32_t ) ( 255.0f * c.blue() ) ) << 16;
					v |= ( ( uint32_t ) ( 255.0f * c.green() ) ) << 8;
					v |= ( ( uint32_t ) ( 255.0f * c.red() ) );
					fillTemplate<uint32_t, uint32_t>( dst, v, &SIMD::SetValueU32 );
				}
				break;
			case IFORMAT_BGRA_UINT8:
				{
					uint32_t v = ( ( uint32_t ) ( 255.0f * c.alpha() ) ) << 24;
					v |= ( ( uint32_t ) ( 255.0f * c.red() ) ) << 16;
					v |= ( ( uint32_t ) ( 255.0f * c.green() ) ) << 8;
					v |= ( ( uint32_t ) ( 255.0f * c.blue() ) );
					fillTemplate<uint32_t, uint32_t>( dst, v, &SIMD::SetValueU32 );
				}
				break;
			case IFORMAT_RGBA_FLOAT:
				{
					float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
					fillTemplate<float, const float ( & )[ 4 ]>( dst, v, &SIMD::SetValue4f );
				}
				break;
			case IFORMAT_BGRA_FLOAT:
				{
					float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
					fillTemplate<float, const float ( & )[ 4 ]>( dst, v, &SIMD::SetValue4f );
				}
				break;
			default:
				throw CVTException("Unimplemented IFill::fill()");
		}
	}


}
