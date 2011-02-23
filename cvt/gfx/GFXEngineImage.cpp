#include <cvt/gfx/GFXEngineImage.h>

namespace cvt {

	void GFXEngineImage::fillRect( const Recti& _rect, const Color& c )
	{
		size_t h, bpp;
		uint8_t* dst;
		SIMD* simd = SIMD::instance();

		Recti rect( 0, 0, _img.width(), _img.height() );
		rect.intersect( _rect );
		if( rect.isEmpty() )
			return;

		bpp = _img.bpp();

		switch ( _img.format().formatID ) {
			case IFORMAT_GRAY_UINT8:
				{
					uint8_t v = ( uint8_t ) ( 255.0f * c.gray() );
					h = rect.height;
					dst = _ptr + rect.y * _stride + bpp * rect.x;
					while( h-- ) {
						simd->SetValueU8( dst, v, rect.width );
						dst += _stride;
					}
				}
				break;
			case IFORMAT_GRAY_UINT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_GRAY_INT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_GRAY_FLOAT:
				{
					float v = c.gray();

					h = rect.height;
					dst = _ptr + rect.y * _stride + bpp * rect.x;
					while( h-- ) {
						simd->SetValue1f( ( float* ) dst, v, rect.width );
						dst += _stride;
					}	h = rect.height;
					dst = _ptr + rect.y * _stride + bpp * rect.x;
					while( h-- ) {
						simd->SetValue1f( ( float* ) dst, v, rect.width );
						dst += _stride;
					}

				}
				break;

			case IFORMAT_GRAYALPHA_UINT8:
				{
					uint16_t v = ( uint16_t ) ( ( ( uint16_t ) ( 255.0f * c.alpha() ) ) << 8 );
					v |= ( ( uint16_t ) ( 255.0f * c.gray() ));

					h = rect.height;
					dst = _ptr + rect.y * _stride + bpp * rect.x;
					while( h-- ) {
						simd->SetValueU16( ( uint16_t* ) dst, v, rect.width );
						dst += _stride;
					}

				}
				break;
			case IFORMAT_GRAYALPHA_UINT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_GRAYALPHA_INT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_GRAYALPHA_FLOAT:
				{
					float v[ 2 ] = { c.gray(), c.alpha() };

					h = rect.height;
					dst = _ptr + rect.y * _stride + bpp * rect.x;
					while( h-- ) {
						simd->SetValue2f( ( float* ) dst, v, rect.width );
						dst += _stride;
					}
				}
				break;
			case IFORMAT_RGBA_UINT8:
				{
					uint32_t v = ( ( uint32_t ) ( 255.0f * c.alpha() ) ) << 24;
					v |= ( ( uint32_t ) ( 255.0f * c.blue() ) ) << 16;
					v |= ( ( uint32_t ) ( 255.0f * c.green() ) ) << 8;
					v |= ( ( uint32_t ) ( 255.0f * c.red() ) );

					h = rect.height;
					dst = _ptr + rect.y * _stride + bpp * rect.x;
					while( h-- ) {
						simd->SetValueU32( ( uint32_t* ) dst, v, rect.width );
						dst += _stride;
					}
				}
				break;
			case IFORMAT_RGBA_UINT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_RGBA_INT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_RGBA_FLOAT:
				{
					float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };

					h = rect.height;
					dst = _ptr + rect.y * _stride + bpp * rect.x;
					while( h-- ) {
						simd->SetValue4f( ( float* ) dst, v, rect.width );
						dst += _stride;
					}
				}
				break;
			case IFORMAT_BGRA_UINT8:
				{
					uint32_t v = ( ( uint32_t ) ( 255.0f * c.alpha() ) ) << 24;
					v |= ( ( uint32_t ) ( 255.0f * c.red() ) ) << 16;
					v |= ( ( uint32_t ) ( 255.0f * c.green() ) ) << 8;
					v |= ( ( uint32_t ) ( 255.0f * c.blue() ) );

					h = rect.height;
					dst = _ptr + rect.y * _stride + bpp * rect.x;
					while( h-- ) {
						simd->SetValueU32( ( uint32_t* ) dst, v, rect.width );
						dst += _stride;
					}
				}
				break;
			case IFORMAT_BGRA_UINT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_BGRA_INT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_BGRA_FLOAT:
				{
					float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };

					h = rect.height;
					dst = _ptr + rect.y * _stride + bpp * rect.x;
					while( h-- ) {
						simd->SetValue4f( ( float* ) dst, v, rect.width );
						dst += _stride;
					}
				}
				break;

			case IFORMAT_BAYER_RGGB_UINT8:
				throw CVTException("Unimplemented");
				break;
			default:
				throw CVTException("Unimplemented");
				break;
		}
	}

}
