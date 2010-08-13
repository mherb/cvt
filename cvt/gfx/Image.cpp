#include "gfx/Image.h"
#include "math/Math.h"

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

    Image::Image( size_t w, size_t h, ImageChannelOrder order, ImageChannelType type ) :  _order( order ), _type( type ), _width( w ), _height( h )
    {

	_stride = Math::pad16( _width ) * _order_channels[ _type ] * _type_size[ _type ];
	posix_memalign( ( void** ) &_data, 16, _stride * _height );
    }

    Image::~Image()
    {
	free( _data );
    }

}
