#include "gfx/Image.h"
#include "gfx/ImageAllocatorMem.h"
#include "math/Math.h"
#include "util/SIMD.h"
#include "util/Exception.h"
#include "util/CVTTest.h"

namespace cvt {

	Image::Image( size_t w, size_t h, IOrder order, IType type ) : IFilterParameter( IFILTERPARAMETER_IMAGE ), _iplimage( 0 )
	{
		_mem = new ImageAllocatorMem();
	    _mem->alloc( w, h, order, type );
		upateIpl();
	}


	Image::Image( const Image& img ) : IFilterParameter( IFILTERPARAMETER_IMAGE ), _iplimage( 0 )
	{
		_mem = new ImageAllocatorMem();
		_mem->copy( img._mem );
	}

	Image::Image( const Image& source, const Recti* roi, bool ref ) : IFilterParameter( IFILTERPARAMETER_IMAGE ), _iplimage( 0 )
	{
		if( !ref ){
			_mem = new ImageAllocatorMem();
			_mem->copy( source._mem, roi );
		} else {
			throw CVTException("Shared image memory not implemented yet");
		}
	}

	void Image::reallocate( size_t w, size_t h, IOrder order, IType type )
	{
		if( _mem->_width == w && _mem->_height == h && _mem->_order == order && _mem->_type == type )
			return;

		_mem->alloc( w, h, order, type );
		upateIpl();
	}

	void Image::copy( const Image& img )
	{
		if( this == &img )
			return;

		checkFormatAndSize( img, __PRETTY_FUNCTION__, __LINE__ );

		size_t cw = _mem->_width * _mem->_order.channels * _mem->_type.size;
		size_t h = _mem->_height;
		uint8_t* dst = map();
		const uint8_t* src = img.map();

		while( h-- ) {
			memcpy( dst, src, cw );
			dst += _mem->_stride;
			src += img._mem->_stride;
		}
		unmap();
		img.unmap();
	}

	void Image::upateIpl()
	{
		/* FIXME: only update data, do not reallocate header */
		if( _iplimage )
			cvReleaseImageHeader( &_iplimage );
		_iplimage = cvCreateImageHeader( cvSize( ( int ) _mem->_width, ( int ) _mem->_height ),
										_mem->_type.id == ICHANNELTYPE_UBYTE ? IPL_DEPTH_8U : IPL_DEPTH_32F, ( int ) _mem->_order.channels );
		cvSetData( _iplimage, map(), ( int ) _mem->_stride );
	}


	Image::~Image()
	{
		delete _mem;
	}

	Image& Image::operator=( const Color& c )
	{
		fill( c );
		return *this;
	}

#if 0	
	Color Image::operator() (int _x, int _y) const
	{
		int x = Math::clamp( _x, ( int ) 0, ( int )( _mem->_width - 1 ) );
		int y = Math::clamp( _y, ( int ) 0, ( int )( _mem->_height - 1 ) );

		switch( _mem->_type ) {
			case CVT_UBYTE:
				{
					uint8_t const * p = ( ( uint8_t const* ) this->scanline( y  ) ) + x * orderChannels[ _mem->_order ];
					switch (_mem->_order) {
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
					float const * p = ( ( float const* ) this->scanline( y ) ) + x * orderChannels[ _mem->_order ];
					switch (_mem->_order) {
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
		x = Math::clamp( x, 0.0f, (float)(_mem->_width-1));
		y = Math::clamp( y, 0.0f, (float)(_mem->_height-1));
		
		int xi = ( int ) Math::floor( x );
		int yi = ( int ) Math::floor( y );
		float alpha = x - ( float ) xi;
		float beta = y - ( float ) yi;
		
		Color c1 = Math::mix( this->operator()( xi, yi ), this->operator()( xi + 1, yi ), alpha );
		Color c2 = Math::mix( this->operator()( xi, yi + 1 ), this->operator()( xi + 1, yi + 1 ), alpha );
				
		return Math::mix( c1, c2, beta );
	}
#endif

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

		out << "Size: " << f.width() << " x " << f.height() << " Channels: " << f.channels() << " Order:" << _order_string[ f.order().id ] << " Type:" << _type_string[ f.type().id ]  << " Stride: " << f.stride() << std::endl;
		return out;
	}
	
	void Image::checkFormat( const Image & img, const char* func, size_t lineNum, IOrder order, IType type ) const
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
		checkFormat(img, func, lineNum, _mem->_order, _mem->_type );
		checkSize(img, func, lineNum, _mem->_width, _mem->_height );
	}

	BEGIN_CVTTEST( image )
		Color color( 255, 0, 0, 255 );
		Image y;
		uint32_t val;
		bool b;
		float* base;

		Image x( 1, 1, IOrder::RGBA, IType::UBYTE );
		x.fill( color );

		std::cerr << "RGBA UBYTE TO:" << std::endl;

		val = *( ( uint32_t* ) x.map() );
		CVTTEST_PRINT("RGBA UBYTE", val == 0xFF0000FF );
		x.unmap();

		x.convert( y, IOrder::BGRA, IType::UBYTE );
		val = *( ( uint32_t* ) y.map() );
		CVTTEST_PRINT("BGRA UBYTE", val == 0xFFFF0000 );
		y.unmap();

		x.convert( y, IOrder::RGBA, IType::FLOAT );
		base = ( float* ) y.map();
		b  = *( base + 0 ) == 1.0f;
		b &= *( base + 1 ) == 0.0f;
		b &= *( base + 2 ) == 0.0f;
		b &= *( base + 3 ) == 1.0f;
		CVTTEST_PRINT("RGBA FLOAT", b );
		y.unmap();

		x.convert( y, IOrder::BGRA, IType::FLOAT );
		base = ( float* ) y.map();
		b  = *( base + 0 ) == 0.0f;
		b &= *( base + 1 ) == 0.0f;
		b &= *( base + 2 ) == 1.0f;
		b &= *( base + 3 ) == 1.0f;
		CVTTEST_PRINT("BGRA FLOAT", b );
		y.unmap();

		x = Image( 1, 1, IOrder::RGBA, IType::FLOAT );
		x.fill( color );
		std::cerr << "RGBA FLOAT TO:" << std::endl;

		x.convert( y, IOrder::RGBA, IType::UBYTE );
		val = *( ( uint32_t* ) y.map() );
		CVTTEST_PRINT("RGBA UBYTE", val == 0xFF0000FF );
		y.unmap();

		x.convert( y, IOrder::BGRA, IType::UBYTE );
		val = *( ( uint32_t* ) y.map() );
		CVTTEST_PRINT("BGRA UBYTE", val == 0xFFFF0000 );
		y.unmap();

		x.convert( y, IOrder::RGBA, IType::FLOAT );
		base = ( float* ) y.map();
		b  = *( base + 0 ) == 1.0f;
		b &= *( base + 1 ) == 0.0f;
		b &= *( base + 2 ) == 0.0f;
		b &= *( base + 3 ) == 1.0f;
		CVTTEST_PRINT("RGBA FLOAT", b );
		y.unmap();

		x.convert( y, IOrder::BGRA, IType::FLOAT );
		base = ( float* ) y.map();
		b  = *( base + 0 ) == 0.0f;
		b &= *( base + 1 ) == 0.0f;
		b &= *( base + 2 ) == 1.0f;
		b &= *( base + 3 ) == 1.0f;
		CVTTEST_PRINT("BGRA FLOAT", b );
		y.unmap();

#if 0
		// operator x, y:
		Image test(2, 2, IOrder::RGBA, IType::UBYTE);		
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
#endif
		return true;
	END_CVTTEST
}
