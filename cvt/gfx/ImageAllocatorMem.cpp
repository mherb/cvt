#include <cvt/gfx/ImageAllocatorMem.h>
#include <cvt/util/SIMD.h>

namespace cvt {

	static inline uint8_t* alignptr( void* _ptr, size_t alignment )
	{
		size_t ptr = ( size_t ) _ptr;
		return ( uint8_t* ) ( ( ptr + ( alignment - 1 ) ) & ( ~ ( alignment - 1 ) ) );
	}

	ImageAllocatorMem::ImageAllocatorMem() : ImageAllocator(), _data( 0 ), _mem( 0 ), _refcnt( 0 )
	{
	}

	ImageAllocatorMem::~ImageAllocatorMem()
	{
		release();
	}

	void ImageAllocatorMem::alloc( size_t width, size_t height, const IFormat & format, uint8_t* data, size_t stride )
	{
		release();
		_width = width;
		_height = height;
		_format = format;

		if( stride == 0 ){
			stride = _width * _format.bpp;
		} else {
			_stride = stride;
		}

		_mem = NULL;
		_data = data;
		_refcnt = new size_t;
		*_refcnt = 0;
		retain();
	}

	void ImageAllocatorMem::alloc( size_t width, size_t height, const IFormat & format )
	{
		release();
		_width = width;
		_height = height;
		_format = format;
		_stride = Math::pad16( _width * _format.bpp );
		_mem = new uint8_t[ _stride * _height + 16 ];
		_data = alignptr( _mem, 16 );
		_refcnt = new size_t;
		*_refcnt = 0;
		retain();
	}

	void ImageAllocatorMem::copy( const ImageAllocator* x, const Recti* r = NULL )
	{
		const uint8_t* src;
		const uint8_t* osrc;
		uint8_t* dst;
		size_t sstride;
		size_t i, n;
		Recti rect( 0, 0, ( int ) x->_width, ( int ) x->_height );
		SIMD* simd = SIMD::instance();

		if( r )
			rect.intersect( *r );

		alloc( rect.width, rect.height, x->_format );

		osrc = src = x->map( &sstride );
		src += rect.y * sstride + x->_format.bpp * rect.x;
		dst = _data;
		n =  _format.bpp * rect.width;

		i = rect.height;
		while( i-- ) {
			simd->Memcpy( dst, src, n );
			dst += _stride;
			src += sstride;
		}
		x->unmap( osrc );
	}

	void ImageAllocatorMem::release()
	{
		if( _refcnt ) {
			*_refcnt--;
			if( *_refcnt <= 0 ) {
				if( _mem )
					delete[] _mem;
				delete _refcnt;
			}
		}
	}

	void ImageAllocatorMem::retain()
	{
		if( _refcnt ) {
			*_refcnt++;
		}
	}

}
