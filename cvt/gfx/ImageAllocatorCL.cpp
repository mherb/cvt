#include <cvt/gfx/ImageAllocatorCL.h>
#include <cvt/util/SIMD.h>

namespace cvt {

	ImageAllocatorCL::ImageAllocatorCL() : ImageAllocator(), _climage( NULL )
	{
	}

	ImageAllocatorCL::~ImageAllocatorCL()
	{
		if( _climage )
			delete _climage;
	}

	void ImageAllocatorCL::alloc( size_t width, size_t height, const IFormat & format )
	{
		if( _width == width && _height == height && _format == format && _climage )
			return;

		if( _climage )
			delete _climage;
		_width = width;
		_height = height;
		_format = format;

		CLImageFormat clformat;
		format.toCLImageFormat( clformat );
		_climage = new CLImage2D( _width, _height, clformat, CL_MEM_READ_WRITE | CL_MEM_HOST_PTR  );
	}

	void ImageAllocatorCL::copy( const ImageAllocator* x, const Recti* r = NULL )
	{
		if( x->type() != IALLOCATOR_CL ) {
			const uint8_t* src;
			const uint8_t* osrc;
			size_t sstride, dstride;
			uint8_t* dst;
			uint8_t* odst;
			size_t i, n;
			Recti rect( 0, 0, ( int ) x->_width, ( int ) x->_height );
			SIMD* simd = SIMD::instance();

			if( r )
				rect.intersect( *r );
			alloc( rect.width, rect.height, x->_format );

			osrc = src = x->map( &sstride );
			src += rect.y * sstride + x->_format.bpp * rect.x;
			odst = dst = map( &dstride );
			n =  x->_format.bpp * rect.width;

			i = rect.height;
			while( i-- ) {
				simd->Memcpy( dst, src, n );
				dst += sstride;
				src += dstride;
			}
			x->unmap( osrc );
			unmap( odst );
		} else {
			const ImageAllocatorCL* clmem = ( ImageAllocatorCL* ) x;
			// FIXME: try to reuse _climage if possible
			if( _climage )
				delete _climage;
			_climage = new CLImage2D( *( clmem->_climage ), r, CL_MEM_READ_WRITE | CL_MEM_HOST_PTR  );
		}
	}

	uint8_t* ImageAllocatorCL::map( size_t* stride )
	{
		return ( uint8_t* ) _climage->map( stride );
	}

	const uint8_t* ImageAllocatorCL::map( size_t* stride ) const
	{
		return ( const uint8_t* ) _climage->map( stride );
	}

	void ImageAllocatorCL::unmap( const uint8_t* ptr ) const
	{
		_climage->unmap( ( const void* ) ptr );
	}

}
