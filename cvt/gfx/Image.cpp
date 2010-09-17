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

	Image::Image( size_t w, size_t h, ImageChannelOrder order, ImageChannelType type ) : IFilterParameter( IFILTERPARAMETER_IMAGE ), _order( order ), _type( type ), _width( w ), _height( h ), _data( 0 ), _iplimage( 0 )
	{
		_stride = Math::pad16( _width * _order_channels[ _order ] * _type_size[ _type ] );
		if( posix_memalign( ( void** ) &_data, 16, _stride * _height ) )
				throw CVTException("Out of memory");
		upateIpl();
	}


	Image::Image( const Image& img ) : IFilterParameter( IFILTERPARAMETER_IMAGE ), _order( CVT_BGRA ), _type( CVT_UBYTE ), _width( 0 ), _height( 0 ), _data( 0 ), _iplimage( 0 )
	{
		reallocate( img );
		copy( img );
	}
	
	Image::Image( const Image& source, Recti roi, bool ref ) : IFilterParameter( IFILTERPARAMETER_IMAGE ), _order( CVT_BGRA ), _type( CVT_UBYTE ), _width( 0 ), _height( 0 ), _data( 0 ), _iplimage( 0 )
	{
		if( !ref ){
			reallocate( roi.width, roi.height, source.order(), source.type() );
			
			// copy the content
			size_t h = _height;
			size_t copyWidth = _width * _order_channels[ _order ] * _type_size[ _type ];
			uint8_t * src = (uint8_t *)source.scanline( roi.y ) + ( _order_channels[ _order ] * _type_size[ _type ] * roi.x );
			uint8_t * dst = _data;
			
			while ( h-- ) {
				memcpy( dst, src, copyWidth );
				dst += _stride;
				src += source.stride();
			}
			
		} else {
			throw CVTException("Shared image memory not implemented yet");
		}
	}
	
	void Image::reallocate( size_t w, size_t h, ImageChannelOrder order, ImageChannelType type )
	{
		if( _width == w && _height == h && _order == order && _type == type )
			return;

		if( _data )
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

		checkFormatAndSize( img, __PRETTY_FUNCTION__, __LINE__ );

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
	
	Color Image::operator() (int _x, int _y) const
	{
		int x = Math::clamp( _x, ( int ) 0, ( int )( _width - 1 ) );
		int y = Math::clamp( _y, ( int ) 0, ( int )( _height - 1 ) );

		switch( _type ) {
			case CVT_UBYTE:
				{
					uint8_t const * p = ( ( uint8_t const* ) this->scanline( y  ) ) + x * _order_channels[ _order ];
					switch (_order) {
						case CVT_RGBA:
							return Color(p[0], p[1], p[2], p[3]);
						case CVT_BGRA:
							return Color(p[2], p[1], p[0], p[3]);
						case CVT_GRAY:
							return Color(p[0]);
						case CVT_GRAYALPHA:
							return Color(p[0], p[1]);
						default:
							throw CVTException("Unsupported Channel Order for UBYTE Type");
							break;
					}
				}
			case CVT_FLOAT:
				{
					float const * p = ( ( float const* ) this->scanline( y ) ) + x * _order_channels[ _order ];
					switch (_order) {
						case CVT_RGBA:
							return Color(p[0], p[1], p[2], p[3]);
						case CVT_BGRA:
							return Color(p[2], p[1], p[0], p[3]);
						case CVT_GRAY:
							return Color(p[0]);
						case CVT_GRAYALPHA:
							return Color(p[0], p[1]);
						default:
							throw CVTException("Unsupported Channel Order for float Type");
							break;
					}
				}
				break;
			default:
				throw CVTException("Not implemented");
				break;
		}
	}
	
	Color Image::operator() ( float x, float y ) const
	{
		x = Math::clamp( x, 0.0f, (float)(_width-1));
		y = Math::clamp( y, 0.0f, (float)(_height-1));
		
		int xi = ( int ) Math::floor( x );
		int yi = ( int ) Math::floor( y );
		float alpha = x - ( float ) xi;
		float beta = y - ( float ) yi;
		
		Color c1 = Math::mix( this->operator()( xi, yi ), this->operator()( xi + 1, yi ), alpha );
		Color c2 = Math::mix( this->operator()( xi, yi + 1 ), this->operator()( xi + 1, yi + 1 ), alpha );
				
		return Math::mix( c1, c2, beta );
	}

	std::ostream& operator<<(std::ostream &out, const Image &f)
	{
		static const char* _order_string[] = {
			"GRAY",
			"GRAYALPHA",
			"RGBA",
			"BGRA"
		};
		static const char* _type_string[] = {
			"UBYTE",
			"FLOAT"
		};

		out << "Size: " << f.width() << " x " << f.height() << " Channels: " << f.channels() << " Order:" << _order_string[ f.order() ] << " Type:" << _type_string[ f.type() ]  << " Stride: " << f.stride() << std::endl;
		return out;
	}
	
	void Image::checkFormat( const Image & img, const char* func, size_t lineNum, ImageChannelOrder order, ImageChannelType type ) const
	{		
		if( order != img.order() ){
			throw Exception("Image formats differ: channel order check failed", "Image", lineNum, func);
		}
		
		if( type != img.type() ){
			throw Exception("Image formats differ: channel type check failed", "Image", lineNum, func);
		}
	}
	
	void Image::checkSize( const Image & img, const char* func, size_t lineNum, size_t w, size_t h ) const
	{
		if( w != img.width() ){			
			throw Exception("Image formats differ: width check failed", "Image", lineNum, func);
		}
		
		if( h != img.height() ){
			throw Exception("Image formats differ: height check failed", "Image", lineNum, func);
		}		
	}
	
	void Image::checkFormatAndSize( const Image & img, const char* func, size_t lineNum ) const
	{
		checkFormat(img, func, lineNum, _order, _type );
		checkSize(img, func, lineNum, _width, _height );
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
