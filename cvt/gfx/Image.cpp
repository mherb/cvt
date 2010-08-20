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
	
	Color Image::operator() (int x, int y)
	{
		int column = Math::clamp( x, (int)0, (int)(_width-1) );
		int row = Math::clamp( y, (int)0, (int)(_height-1) );
		switch (_type) {
			case CVT_UBYTE:
			{
				uint8_t * p = ((uint8_t*)this->scanline(row)) + column * _order_channels[_order] * _type_size[_type];
				switch (_order) {
					case CVT_RGBA:
						return Color(p[0], p[1], p[2], p[3]);
						break;
					case CVT_BGRA:
						return Color(p[2], p[1], p[0], p[3]);
						break;
					case CVT_GRAY:
						return Color(p[0]);
						break;
					case CVT_GRAYALPHA:
						return Color(p[0], p[1]);
						break;					
					default:
						throw CVTException("Unsupported Channel Order for UBYTE Type");
						break;
				}
			}
			case CVT_FLOAT:
			{
				float * p = ((float*)this->scanline(row)) + column * _order_channels[_order] * _type_size[_type];
				switch (_order) {
					case CVT_RGBA:
						return Color(p[0], p[1], p[2], p[3]);
						break;
					case CVT_BGRA:
						return Color(p[2], p[1], p[0], p[3]);
						break;
					case CVT_GRAY:
						return Color(p[0]);
						break;
					case CVT_GRAYALPHA:
						return Color(p[0], p[1]);
						break;					
					default:
						throw CVTException("Unsupported Channel Order for float Type");
						break;
				}
			}			
			default:
				throw CVTException("Not implemented");
				break;
		}
	}
	
	Color Image::operator() ( float x, float y )
	{
		x = Math::clamp(x, 0.0f, (float)(_width-1));
		y = Math::clamp(y, 0.0f, (float)(_height-1));
		
		int r = (int)floor( y );
		int c = (int)floor( x );
		float alpha = y - r;
		float beta = x - c;
		
		Color c1 = Math::mix( this->operator()( r, c ), this->operator()( r, c+1 ), alpha );
		Color c2 = Math::mix( this->operator()( r+1, c ), this->operator()( r+1, c+1 ), alpha );
				
		return Math::mix( c1, c2, beta );
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

		// operator x, y:
		Image test(2, 2, CVT_RGBA, CVT_UBYTE);		
		test.fill(color);
		Color c = test(0, 0);
		b = (c.red() == 1.0f);
		b &= (c.green() == 0.0f);
		b &= (c.blue() == 0.0f);
		b &= (c.alpha() == 1.0f);
		CVTTEST_PRINT("INDEX OPERATOR: ", b );
		
		uint8_t * p = test.scanline(1);
		p[0] = 0; p[1] = 255; p[2] = 0;	p[3] = 255;	
		p[4] = 0; p[5] = 255; p[6] = 0;	p[7] = 255;	
		cvt::Color cback = test(0.2f, 0.2f);
		b = (cback.red() == 0.8f);
		b &= (cback.green() == 0.2f);
		b &= (cback.blue() == 0.0f);
		b &= (cback.alpha() == 1.0f);
		CVTTEST_PRINT("PIXEL INTERPOLATOR: ", b );
	
		return true;
	END_CVTTEST
}
