#include "gfx/Image.h"
#include "math/Math.h"
#include "util/SIMD.h"
#include "util/Exception.h"

namespace cvt {
    
    size_t Image::_type_size[] = {
	sizeof( uint8_t ), /* CVT_UBYTE */
	sizeof( float )    /* CVT_FLOAT */
    };
    
    size_t Image::_order_channels[ ] = {
	1, /* CVT_GRAY */
	2, /* CVT_GRAYALPHA */
	4, /* CVT_RGBA */
	4  /* CVT_BGRA */
    };
    
    Image::Image( size_t w, size_t h, ImageChannelOrder order, ImageChannelType type ) :  _order( order ), _type( type ), _width( w ), _height( h ), _data( 0 ), _iplimage( 0 )
    {
	_stride = Math::pad16( _width * _order_channels[ _order ] * _type_size[ _type ] );
	posix_memalign( ( void** ) &_data, 16, _stride * _height );
	upateIpl();
    }
    
    
    Image::Image( const Image& img ) : _order( CVT_BGRA ), _type( CVT_UBYTE ), _width( 0 ), _height( 0 ), _data( 0 ), _iplimage( 0 )
    {
	copy( img );
    }
    
    
    size_t Image::channels() const
    {
	return _order_channels[ _order ];
    }
    
    size_t Image::bpc() const
    {
	return _type_size[_type] * 8;
    }
    
    size_t Image::bpp() const
    {
	return _type_size[_type] * _order_channels[_type];
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
	_stride = Math::pad16( _width * _order_channels[ _order ] * _type_size[ _type ] );
	posix_memalign( ( void** ) &_data, 16, _stride * _height );
	upateIpl();
    }
    
    void Image::copy( const Image& img )
    {
	if( this == &img )
	    return;
	
	reallocate( img._width, img._height, img._order, img._type );
	
	size_t cw = _width * _order_channels[ _order ] * _type_size[ _type ];
	size_t h = _height;
	uint8_t* dst = _data;
	uint8_t* src = img._data;
	
	while( h-- ) {
	    memcpy( dst, src, cw );
	    dst += _stride;
	    src += img._stride;
	}
    }
    
    
    void Image::upateIpl()
    {
	/* FIXME: only update data, do not reallocate header */
	if( _iplimage )
	    cvReleaseImageHeader( &_iplimage );
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
    
    Image& Image::operator=( const Color& c )
    {
	fill( c );
	return *this;
    }
    
    std::ostream& operator<<(std::ostream &out, const Image &f)
    {
	out << "Size: " << f.width() << " x " << f.height() << " Channels: " << f.channels() << " Stride: " << f.stride() << std::endl;
	return out;
    }
    
}
