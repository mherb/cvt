#include "gfx/Image.h"
#include "math/Math.h"
#include "util/SIMD.h"

namespace cvt {

    static size_t _type_size[] = {
			    sizeof( uint8_t ), /* CVT_UBYTE */
			    sizeof( float )    /* CVT_FLOAT */
			  };

    static size_t _order_channels[ ] = {
				    1, /* CVT_GRAY */
				    2, /* CVT_GRAYALPHA */
				    4, /* CVT_RGBA */
				    4  /* CVT_BGRA */
				};

    Image::Image( size_t w, size_t h, ImageChannelOrder order, ImageChannelType type ) :  _order( order ), _type( type ), _width( w ), _height( h ), _iplimage( 0 )
    {

	_stride = Math::pad16( _width ) * _order_channels[ _order ] * _type_size[ _type ];
	posix_memalign( ( void** ) &_data, 16, _stride * _height );
	upateIpl();
    }


    Image::Image( const Image& img )
    {
	std::cout << "COPY" << std::endl;
	_order = img._order;
	_type = img._type;
	_width = img._width;
	_height = img._height;
	_stride = Math::pad16( _width ) * _order_channels[ _order ] * _type_size[ _type ];
	_iplimage = 0;
	posix_memalign( ( void** ) &_data, 16, _stride * _height );

	size_t h = _height;
	uint8_t* dst = _data;
	uint8_t* src = img._data;
	while( h-- ) {
	    memcpy( dst, src, _width );
	    dst += _stride;
	    src += img._stride;
	}
	upateIpl();
    }


    void Image::reallocate( size_t w, size_t h, ImageChannelOrder order, ImageChannelType type )
    {
	if( _width == w && _height == h && _order == order && _type == type )
	    return;

	free( _data );
	_order = order;
	_type = type;
	_width = w;
	_height = h;
	_stride = Math::pad16( _width ) * _order_channels[ _order ] * _type_size[ _type ];
	posix_memalign( ( void** ) &_data, 16, _stride * _height );
	upateIpl();
    }


    void Image::upateIpl()
    {
	if( _iplimage )
	    cvReleaseImage( &_iplimage );
	_iplimage = cvCreateImageHeader( cvSize( ( int ) _width, ( int ) _height ),
					_type == CVT_UBYTE ? IPL_DEPTH_8U : IPL_DEPTH_32F, ( int ) _order_channels[ _order ] );
	cvSetData( _iplimage, _data, ( int ) _stride );
    }

    Image::~Image()
    {
	if( _iplimage )
	    cvReleaseImageHeader( &_iplimage );
	free( _data );
    }

    void Image::fill( const Color& c )
    {
	size_t h;
	uint8_t* dst;
	SIMD* simd = SIMD::get();

	switch( _order ) {
	    case CVT_GRAY:
		{
		    uint8_t v = ( uint8_t ) ( 255.0f * c.gray() );
		    h = _height;
		    dst = _data;
		    while( h-- ) {
			simd->set_value_u8( dst, _width, v );
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
			simd->set_value_u16( ( uint16_t* ) dst, _width, v );
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
			simd->set_value_u32( ( uint32_t* ) dst, _width, v );
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
			simd->set_value_u32( ( uint32_t* ) dst, _width, v );
			dst += _stride;
		    }
		}
		break;
	}
    }

    Image& Image::operator=( const Color& c )
    {
	fill( c );
	return *this;
    }

    std::ostream& operator<<(std::ostream &out, const Image &f)
    {
	out << "Size: " << f.width() << " x " << f.height() << " Channels: " << _order_channels[ f._order ] << " Stride: " << f.stride() << std::endl;
	return out;
    }

}
