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

	void ImageAllocatorMem::alloc( size_t width, size_t height, const IOrder order, const IType type )
	{
		release();
		_width = width;
		_height = height;
		_order = order;
		_type = type;
		_stride = Math::pad16( _width * _order.channels* _type.size );
		_mem = new uint8_t[ _stride * _height + 16 ];
		_data = alignptr( _mem, 16 );
		_refcnt = new size_t;
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
		SIMD* simd = SIMD::get();

		if( r )
			rect.intersect( *r );
		std::cout << "Rect: " << rect << std::endl;

		alloc( rect.width, rect.height, x->_order, x->_type );

		osrc = src = x->map( &sstride );
		src += rect.y * sstride + x->_type.size * x->_order.channels * rect.x;
		dst = _data;
		n =  x->_type.size * x->_order.channels * rect.width;

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
