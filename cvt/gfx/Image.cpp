#include "gfx/Image.h"
#include "math/Math.h"
#include "util/SIMD.h"
#include "util/Exception.h"
#include "util/CVTTest.h"

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
		if( posix_memalign( ( void** ) &_data, 16, _stride * _height ) )
				throw CVTException("Out of memory");
		upateIpl();
	}


	Image::Image( const Image& img ) : _order( CVT_BGRA ), _type( CVT_UBYTE ), _width( 0 ), _height( 0 ), _data( 0 ), _iplimage( 0 )
	{
		copy( img );
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
		if( posix_memalign( ( void** ) &_data, 16, _stride * _height ) )
				throw CVTException("Out of memory");
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


	BEGIN_CVTTEST( image )
		Color color( 255, 0, 0, 255 );
		Image y;
		uint32_t val;
		bool b;

		Image x( 1, 1, CVT_RGBA, CVT_UBYTE );
		x.fill( color );

		std::cerr << "RGBA UBYTE TO:" << std::endl;

		val = *( ( uint32_t* ) x.data() );
		CVTTEST_PRINT("RGBA UBYTE", val == 0xFF0000FF );

		x.convert( y, CVT_BGRA, CVT_UBYTE );
		val = *( ( uint32_t* ) y.data() );
		CVTTEST_PRINT("BGRA UBYTE", val == 0xFFFF0000 );

		x.convert( y, CVT_RGBA, CVT_FLOAT );
		b = *( ( ( float* ) y.data() ) + 0 ) == 1.0f;
		b &= *( ( ( float* ) y.data() ) + 1 ) == 0.0f;
		b &= *( ( ( float* ) y.data() ) + 2 ) == 0.0f;
		b &= *( ( ( float* ) y.data() ) + 3 ) == 1.0f;
		CVTTEST_PRINT("RGBA FLOAT", b );

		x.convert( y, CVT_BGRA, CVT_FLOAT );
		b = *( ( ( float* ) y.data() ) + 0 ) == 0.0f;
		b &= *( ( ( float* ) y.data() ) + 1 ) == 0.0f;
		b &= *( ( ( float* ) y.data() ) + 2 ) == 1.0f;
		b &= *( ( ( float* ) y.data() ) + 3 ) == 1.0f;
		CVTTEST_PRINT("BGRA FLOAT", b );

		x = Image( 1, 1, CVT_RGBA, CVT_FLOAT );
		x.fill( color );
		std::cerr << "RGBA FLOAT TO:" << std::endl;

		x.convert( y, CVT_RGBA, CVT_UBYTE );
		val = *( ( uint32_t* ) y.data() );
		CVTTEST_PRINT("RGBA UBYTE", val == 0xFF0000FF );

		x.convert( y, CVT_BGRA, CVT_UBYTE );
		val = *( ( uint32_t* ) y.data() );
		CVTTEST_PRINT("BGRA UBYTE", val == 0xFFFF0000 );

		x.convert( y, CVT_RGBA, CVT_FLOAT );
		b = *( ( ( float* ) y.data() ) + 0 ) == 1.0f;
		b &= *( ( ( float* ) y.data() ) + 1 ) == 0.0f;
		b &= *( ( ( float* ) y.data() ) + 2 ) == 0.0f;
		b &= *( ( ( float* ) y.data() ) + 3 ) == 1.0f;
		CVTTEST_PRINT("RGBA FLOAT", b );

		x.convert( y, CVT_BGRA, CVT_FLOAT );
		b = *( ( ( float* ) y.data() ) + 0 ) == 0.0f;
		b &= *( ( ( float* ) y.data() ) + 1 ) == 0.0f;
		b &= *( ( ( float* ) y.data() ) + 2 ) == 1.0f;
		b &= *( ( ( float* ) y.data() ) + 3 ) == 1.0f;
		CVTTEST_PRINT("BGRA FLOAT", b );


		return true;
	END_CVTTEST
}
