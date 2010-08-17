#include "gfx/Image.h"
#include "math/Math.h"
#include "util/SIMD.h"
#include "util/CVTException.h"

namespace cvt {
    void Image::convert( Image& img, ImageChannelOrder order, ImageChannelType type ) const
    {
	SIMD* simd = SIMD::get();
	img.reallocate( _width, _height, order, type );

	if( order != _order )
	    throw CVTException( "Unimplemented" );

	/* same order - different type */
	if( _type == CVT_UBYTE && type == CVT_FLOAT ) {
	    uint8_t* src = _data;
	    uint8_t* dst = img._data;
	    size_t h = _height;

	    while( h-- ) {
		simd->Conv_u8_to_f( ( float* ) dst, src, _width * _order_channels[ _order ] );
		src += _stride;
		dst += img._stride;
	    }
	} else if( _type == CVT_FLOAT && type == CVT_UBYTE ) {
	    uint8_t* src = _data;
	    uint8_t* dst = img._data;

	    size_t h = _height;
	    while( h-- ) {
		simd->Conv_f_to_u8( dst, ( float* ) src, _width * _order_channels[ _order ] );
		src += _stride;
		dst += img._stride;
	    }
	}
    }

    void Image::fill( const Color& c )
    {
	size_t h;
	uint8_t* dst;
	SIMD* simd = SIMD::get();

	if( _type == CVT_UBYTE ) {
	    switch( _order ) {
		case CVT_GRAY:
		    {
			uint8_t v = ( uint8_t ) ( 255.0f * c.gray() );
			h = _height;
			dst = _data;
			while( h-- ) {
			    simd->SetValueU8( dst, v, _width );
			    dst += _stride;
			}
		    }
		    break;
		case CVT_GRAYALPHA:
		    {
			uint16_t v = ( ( uint16_t ) ( 255.0f * c.alpha() ) ) << 8;
			v |= ( ( uint16_t ) ( 255.0f * c.gray() ));
			h = _height;
			dst = _data;
			while( h-- ) {
			    simd->SetValueU16( ( uint16_t* ) dst, v, _width );
			    dst += _stride;
			}
		    }
		    break;
		case CVT_RGBA:
		    {
			uint32_t v = ( ( uint32_t ) ( 255.0f * c.gray() ) ) << 24;
			v |= ( ( uint32_t ) ( 255.0f * c.blue() ) ) << 16;
			v |= ( ( uint32_t ) ( 255.0f * c.green() ) ) << 8;
			v |= ( ( uint32_t ) ( 255.0f * c.red() ) );

			h = _height;
			dst = _data;
			while( h-- ) {
			    simd->SetValueU32( ( uint32_t* ) dst, v, _width );
			    dst += _stride;
			}
		    }
		    break;
		case CVT_BGRA:
		    {
			uint32_t v = ( ( uint32_t ) ( 255.0f * c.gray() ) ) << 24;
			v |= ( ( uint32_t ) ( 255.0f * c.red() ) ) << 16;
			v |= ( ( uint32_t ) ( 255.0f * c.green() ) ) << 8;
			v |= ( ( uint32_t ) ( 255.0f * c.blue() ) );

			h = _height;
			dst = _data;
			while( h-- ) {
			    simd->SetValueU32( ( uint32_t* ) dst, v, _width );
			    dst += _stride;
			}
		    }
		    break;
	    }
	} else if( _type == CVT_FLOAT ) {
	    switch( _order ) {
		case CVT_GRAY:
		    {
			float v = c.gray();

			h = _height;
			dst = _data;
			while( h-- ) {
			    simd->SetValue1f( ( float* ) dst, v, _width );
			    dst += _stride;
			}
		    }
		    break;
		case CVT_GRAYALPHA:
		    {
			float v[ 2 ] = { c.gray(), c.alpha() };

			h = _height;
			dst = _data;
			while( h-- ) {
			    simd->SetValue2f( ( float* ) dst, v, _width );
			    dst += _stride;
			}
		    }
		    break;
		case CVT_RGBA:
		    {
			float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };

			h = _height;
			dst = _data;
			while( h-- ) {
			    simd->SetValue4f( ( float* ) dst, v, _width );
			    dst += _stride;
			}
		    }
		    break;
		case CVT_BGRA:
		    {
			float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };

			h = _height;
			dst = _data;
			while( h-- ) {
			    simd->SetValue4f( ( float* ) dst, v, _width );
			    dst += _stride;
			}
		    }
		    break;

	    }
	} else
	    throw CVTException("Unimplemented");
    }

    void Image::add( float alpha )
    {
	SIMD* simd = SIMD::get();
	switch( _type ) {
	    case CVT_FLOAT:
		{
		    uint8_t* dst = _data;

		    size_t h = _height;
		    while( h-- ) {
			simd->Add( ( float* ) dst, ( float* ) dst, alpha, _width * _order_channels[ _order ] );
			dst += _stride;
		    }
		}
		break;
	    default:
		throw CVTException("Unimplemented");

	}
    }


    void Image::add( const Color& c )
    {
	SIMD* simd = SIMD::get();
	switch( _type ) {
	    case CVT_FLOAT:
		{
		    switch( _order ) {
			case CVT_RGBA:
			    {
				float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
				uint8_t* dst = _data;
				size_t h = _height;

				while( h-- ) {
				    simd->Add( ( float* ) dst, ( float* ) dst, v, _width *_order_channels[ _order ] );
				    dst += _stride;
				}
			    }
			    break;
			case CVT_BGRA:
			    {
				float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
				uint8_t* dst = _data;
				size_t h = _height;

				while( h-- ) {
				    simd->Add( ( float* ) dst, ( float* ) dst, v, _width *_order_channels[ _order ] );
				    dst += _stride;
				}
			    }
			    break;
		    }
		}
		break;
	    default:
		throw CVTException("Unimplemented");

	}
    }

    void Image::sub( float alpha )
    {
	SIMD* simd = SIMD::get();
	switch( _type ) {
	    case CVT_FLOAT:
		{
		    uint8_t* dst = _data;

		    size_t h = _height;
		    while( h-- ) {
			simd->Sub( ( float* ) dst, ( float* ) dst, alpha, _width * _order_channels[ _order ] );
			dst += _stride;
		    }
		}
		break;
	    default:
		throw CVTException("Unimplemented");

	}
    }

    void Image::sub( const Color& c )
    {
	SIMD* simd = SIMD::get();
	switch( _type ) {
	    case CVT_FLOAT:
		{
		    switch( _order ) {
			case CVT_RGBA:
			    {
				float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
				uint8_t* dst = _data;
				size_t h = _height;

				while( h-- ) {
				    simd->Sub( ( float* ) dst, ( float* ) dst, v, _width *_order_channels[ _order ] );
				    dst += _stride;
				}
			    }
			    break;
			case CVT_BGRA:
			    {
				float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
				uint8_t* dst = _data;
				size_t h = _height;

				while( h-- ) {
				    simd->Sub( ( float* ) dst, ( float* ) dst, v, _width *_order_channels[ _order ] );
				    dst += _stride;
				}
			    }
			    break;
		    }
		}
		break;
	    default:
		throw CVTException("Unimplemented");

	}
    }

    void Image::mul( float alpha )
    {
	SIMD* simd = SIMD::get();
	switch( _type ) {
	    case CVT_FLOAT:
		{
		    uint8_t* dst = _data;

		    size_t h = _height;
		    while( h-- ) {
			simd->Mul( ( float* ) dst, ( float* ) dst, alpha, _width * _order_channels[ _order ] );
			dst += _stride;
		    }
		}
		break;
	    default:
		throw CVTException("Unimplemented");

	}
    }

    void Image::mul( const Color& c )
    {
	SIMD* simd = SIMD::get();
	switch( _type ) {
	    case CVT_FLOAT:
		{
		    switch( _order ) {
			case CVT_RGBA:
			    {
				float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
				uint8_t* dst = _data;
				size_t h = _height;

				while( h-- ) {
				    simd->Mul( ( float* ) dst, ( float* ) dst, v, _width *_order_channels[ _order ] );
				    dst += _stride;
				}
			    }
			    break;
			case CVT_BGRA:
			    {
				float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
				uint8_t* dst = _data;
				size_t h = _height;

				while( h-- ) {
				    simd->Mul( ( float* ) dst, ( float* ) dst, v, _width *_order_channels[ _order ] );
				    dst += _stride;
				}
			    }
			    break;
		    }
		}
		break;
	    default:
		throw CVTException("Unimplemented");

	}
    }

    void Image::mad( const Image& i, float alpha )
    {
	if( _width != i._width || _height != i._height ||
	   _type != i._type || _order != i._order )
	    throw CVTException("Image mismatch");

	SIMD* simd = SIMD::get();
	switch( _type ) {
	    case CVT_FLOAT:
		{
		    uint8_t* src = i._data;
		    uint8_t* dst = _data;

		    size_t h = _height;
		    while( h-- ) {
			simd->MulAdd( ( float* ) dst, ( float* ) src, alpha, _width * _order_channels[ _order ] );
			src += i._stride;
			dst += _stride;
		    }
		}
		break;
	    default:
		throw CVTException("Unimplemented");

	}
    }

    void Image::ddx( Image& dx, bool forward ) const
    {
	SIMD* simd = SIMD::get();

	dx.reallocate( *this );

	switch( _type ) {
	    case CVT_FLOAT:
		{
		    size_t h = _height;
		    uint8_t* dst = dx._data;
		    float* fdst;
		    uint8_t* src1 = _data;
		    uint8_t* src2 = _data + sizeof( float ) * _order_channels[ _order ];
		    size_t c = _order_channels[ _order ];
		    size_t i;
		    while( h-- ) {
			i = c;
			fdst = ( float* ) dst;
			if( !forward ) {
			    while( i-- )
				*fdst++ = 0.0f;
			}
			simd->Sub( fdst, ( float* ) src1, ( float* ) src2, ( _width - 1 ) * _order_channels[ _order ] );
			if( forward ) {
			    fdst += ( _width - 1 ) * _order_channels[ _order ];
			    while( i-- )
				*fdst++ = 0.0f;
			}
			dst += dx._stride;
			src1 += _stride;
			src2 += _stride;
		    }
		}
		break;
	    default:
		throw CVTException("Unimplemented");
	}
    }

    void Image::ddy( Image& dy, bool forward ) const
    {
	SIMD* simd = SIMD::get();

	dy.reallocate( *this );

	switch( _type ) {
	    case CVT_FLOAT:
		{
		    size_t h = _height - 1;
		    uint8_t* dst = dy._data;
		    uint8_t* src1 = _data;
		    uint8_t* src2 = _data + _stride;
		    if( !forward ) {
			simd->SetValue1f( ( float* ) dst, 0.0f, _width * _order_channels[ dy._order ] );
			dst += dy._stride;
		    }
		    while( h-- ) {
			simd->Sub( ( float* ) dst, ( float* ) src1, ( float* ) src2, _width * _order_channels[ dy._order ] );
			dst += dy._stride;
			src1 += _stride;
			src2 += _stride;
		    }
		    if( forward )
			simd->SetValue1f( ( float* ) dst, 0.0f, _width * _order_channels[ dy._order ] );
		}
		break;
	    default:
		throw CVTException("Unimplemented");
	}
    }
}
