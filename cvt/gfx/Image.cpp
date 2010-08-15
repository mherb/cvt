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

    Image::Image( size_t w, size_t h, ImageChannelOrder order, ImageChannelType type ) :  _order( order ), _type( type ), _width( w ), _height( h ), _iplimage( 0 )
    {

	_stride = Math::pad16( _width ) * _order_channels[ _order ] * _type_size[ _type ];
	posix_memalign( ( void** ) &_data, 16, _stride * _height );
	upateIpl();
    }


    void Image::reallocate( size_t w, size_t h, ImageChannelOrder order, ImageChannelType type )
    {
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
	    cvReleaseImage( &_iplimage );
	free( _data );
    }


    std::ostream& operator<<(std::ostream &out, const Image &f)
    {
	out << "Size: " << f.width() << " x " << f.height() << " Channels: " << _order_channels[ f._order ] << " Stride: " << f.stride() << std::endl;
	return out;
    }

}
