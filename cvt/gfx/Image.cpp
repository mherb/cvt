#include "gfx/Image.h"
#include "gfx/ImageAllocatorMem.h"
#include "gfx/ImageAllocatorCL.h"
#include "gfx/ImageAllocatorGL.h"
#include "math/Math.h"
#include "util/SIMD.h"
#include "util/Exception.h"
#include "util/CVTTest.h"

namespace cvt {

	Image::Image( size_t w, size_t h, const IFormat & format, IAllocatorType memtype ) : IFilterParameter( IFILTERPARAMETER_IMAGE )
	{
		if( memtype == IALLOCATOR_CL )
			_mem = new ImageAllocatorCL();
		else if( memtype == IALLOCATOR_GL )
			_mem = new ImageAllocatorGL();
		else
			_mem = new ImageAllocatorMem();
	    _mem->alloc( w, h, format );
	}


	Image::Image( size_t w, size_t h, const IFormat & format, uint8_t* data, size_t stride ) : IFilterParameter( IFILTERPARAMETER_IMAGE )
	{
		_mem = new ImageAllocatorMem();
		ImageAllocatorMem * memAllocator = (ImageAllocatorMem *)_mem;
		memAllocator->alloc( w, h, format, data, stride );
	}


	Image::Image( const Image& img, IAllocatorType memtype ) : IFilterParameter( IFILTERPARAMETER_IMAGE )
	{
		if( memtype == IALLOCATOR_CL )
			_mem = new ImageAllocatorCL();
		else if( memtype == IALLOCATOR_GL )
			_mem = new ImageAllocatorGL();
		else
			_mem = new ImageAllocatorMem();
		_mem->copy( img._mem );
	}

	Image::Image( const Image& source, const Recti* roi, bool ref, IAllocatorType memtype ) : IFilterParameter( IFILTERPARAMETER_IMAGE )
	{
		if( !ref ){
			if( memtype == IALLOCATOR_CL )
				_mem = new ImageAllocatorCL();
			else if( memtype == IALLOCATOR_GL )
				_mem = new ImageAllocatorGL();
			else
				_mem = new ImageAllocatorMem();
			_mem->copy( source._mem, roi );
		} else {
			throw CVTException("Shared image memory not implemented yet");
		}
	}

	void Image::reallocate( size_t w, size_t h, const IFormat & format, IAllocatorType memtype )
	{
		if( _mem->_width == w && _mem->_height == h && _mem->_format == format && _mem->type() == memtype )
			return;
		if( _mem->type() != memtype ) {
			delete _mem;
			if( memtype == IALLOCATOR_CL )
				_mem = new ImageAllocatorCL();
			else if( memtype == IALLOCATOR_GL )
				_mem = new ImageAllocatorGL();
			else
				_mem = new ImageAllocatorMem();
		}
		_mem->alloc( w, h, format );
	}

	void Image::copy( const Image& img )
	{
		if( this == &img )
			return;

		checkFormatAndSize( img, __PRETTY_FUNCTION__, __LINE__ );
		_mem->copy( img._mem );
	}

	void Image::copyRect( int x, int y, const Image& img, int sx, int sy, int swidth, int sheight )
	{
		checkFormat( img, __PRETTY_FUNCTION__, __LINE__, _mem->_format );
		int tx, ty;

		tx = -x + sx;
		ty = -y + sy;
		Recti rdst( 0, 0, ( int ) _mem->_width, ( int ) _mem->_height );
		rdst.translate( tx, ty );
		Recti rsrc( 0, 0, ( int ) img._mem->_width, ( int ) img._mem->_height );
		rsrc.intersect( sx, sy, swidth, sheight );
		rsrc.intersect( rdst );
		if( rsrc.isEmpty() )
			return;
		rdst.copy( rsrc );
		rdst.translate( -tx, -ty );

		SIMD* simd = SIMD::get();
		size_t dstride;
		uint8_t* dst = map( &dstride );
		uint8_t* dbase = dst;
		dst += rdst.y * dstride + bpp() * rdst.x;

		size_t sstride;
		const uint8_t* src = img.map( &sstride );
		const uint8_t* sbase = src;
		src += rsrc.y * sstride + rsrc.x * img.bpp();

		size_t n = rsrc.width * img.bpp();
		size_t i = rsrc.height;

		while( i-- ) {
			simd->Memcpy( dst, src, n );
			src += sstride;
			dst += dstride;
		}
		img.unmap( sbase );
		unmap( dbase );
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

	std::ostream& operator<<( std::ostream &out, const Image &f )
	{		
		static const char* _mem_string[] = {
			"MEM",
			"CL",
			"GL"
		};


		out << "Size: " << f.width() << " x " << f.height() << " "
			<< f.format()
			<< " Memory:" << _mem_string[ f.memType() ] << std::endl;
		return out;
	}
	
	void Image::checkFormat( const Image & img, const char* func, size_t lineNum, const IFormat & format ) const
	{		
		if( format != img.format() ){
			throw Exception("Image formats differ: channel order check failed", "Image", lineNum, func);
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
		checkFormat(img, func, lineNum, _mem->_format );
		checkSize(img, func, lineNum, _mem->_width, _mem->_height );
	}

	BEGIN_CVTTEST( image )
		Color color( 255, 0, 0, 255 );
		Image y;
		uint32_t val;
		uint8_t* valPtr;
		bool b;
		float* base;
		size_t stride;

		Image x( 1, 1, IFormat::RGBA_UINT8 );
		x.fill( color );

		std::cerr << "RGBA UBYTE TO:" << std::endl;

		valPtr = x.map( &stride );
		val = *( ( uint32_t* ) valPtr );
		CVTTEST_PRINT("RGBA UBYTE", val == 0xFF0000FF );
		x.unmap( valPtr );

		x.convert( y, IFormat::BGRA_UINT8 );
		valPtr = y.map( &stride );
		val = *( ( uint32_t* ) valPtr );
		CVTTEST_PRINT("BGRA UBYTE", val == 0xFFFF0000 );
		y.unmap( valPtr );

		x.convert( y, IFormat::RGBA_FLOAT );
		base = ( float* ) y.map( &stride );
		b  = *( base + 0 ) == 1.0f;
		b &= *( base + 1 ) == 0.0f;
		b &= *( base + 2 ) == 0.0f;
		b &= *( base + 3 ) == 1.0f;
		CVTTEST_PRINT("RGBA FLOAT", b );
		y.unmap( ( uint8_t* ) base );

		x.convert( y, IFormat::BGRA_FLOAT );
		base = ( float* ) y.map( &stride );
		b  = *( base + 0 ) == 0.0f;
		b &= *( base + 1 ) == 0.0f;
		b &= *( base + 2 ) == 1.0f;
		b &= *( base + 3 ) == 1.0f;
		CVTTEST_PRINT("BGRA FLOAT", b );
		y.unmap( ( uint8_t* ) base );

		x.reallocate( 1, 1, IFormat::BGRA_FLOAT );
		x.fill( color );
		std::cerr << "RGBA FLOAT TO:" << std::endl;

		y.reallocate( 1, 1, IFormat::RGBA_UINT8 );
		x.convert( y );
		
		valPtr = y.map( &stride );
		val = *( ( uint32_t* ) valPtr );		
		CVTTEST_PRINT("RGBA UBYTE", val == 0xFF0000FF );
		y.unmap( valPtr );

		y.reallocate( 1, 1, IFormat::BGRA_UINT8 );
		x.convert( y );
		valPtr = y.map( &stride );
		val = *( ( uint32_t* ) valPtr );		
		CVTTEST_PRINT("BGRA UBYTE", val == 0xFFFF0000 );
		y.unmap( valPtr );

		y.reallocate( 1, 1, IFormat::RGBA_FLOAT );
		x.convert( y );
		base = ( float* ) y.map( &stride );
		b  = *( base + 0 ) == 1.0f;
		b &= *( base + 1 ) == 0.0f;
		b &= *( base + 2 ) == 0.0f;
		b &= *( base + 3 ) == 1.0f;
		CVTTEST_PRINT("RGBA FLOAT", b );
		y.unmap( ( uint8_t* ) base );

		y.reallocate( 1, 1, IFormat::BGRA_FLOAT );
		x.convert( y );
		base = ( float* ) y.map( &stride );
		b  = *( base + 0 ) == 0.0f;
		b &= *( base + 1 ) == 0.0f;
		b &= *( base + 2 ) == 1.0f;
		b &= *( base + 3 ) == 1.0f;
		CVTTEST_PRINT("BGRA FLOAT", b );
		y.unmap( ( uint8_t* ) base );

#if 0
		// operator x, y:
		Image test(2, 2, IFormat::RGBA_UINT8);		
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
