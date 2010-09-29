#include <cvt/gfx/ImageAllocatorCL.h>
#include <cvt/util/SIMD.h>

namespace cvt {

	ImageAllocatorCL::ImageAllocatorCL() : ImageAllocator(), _cl( 0 ), _climage( 0 )
	{
	}

	ImageAllocatorCL::~ImageAllocatorCL()
	{
		if( _climage )
			delete _climage;
	}

	void ImageAllocatorCL::alloc( size_t width, size_t height, const IOrder order, const IType type )
	{
		cl_int err;

		if( _climage )
			delete _climage;
		_width = width;
		_height = height;
		_order = order;
		_type = type;
		_cl = CLContext::getCurrent();
		_climage = new cl::Image2D( _cl->getCLContext( ), CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, getCLFormat( _order, _type ), _width, _height, 0, NULL, &err );
		if( err != CL_SUCCESS )
			throw CLException( __PRETTY_FUNCTION__, err );
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
			SIMD* simd = SIMD::get();

			if( r )
				rect.intersect( *r );
			alloc( rect.width, rect.height, x->_order, x->_type );

			osrc = src = x->map( &sstride );
			src += rect.y * sstride + x->_type.size * x->_order.channels * rect.x;
			odst = dst = map( &dstride );
			n =  x->_type.size * x->_order.channels * rect.width;

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
			cl_int err;
			cl::size_t<3> src;
			cl::size_t<3> dst;
			cl::size_t<3> crect;
			Recti rect( 0, 0, ( int ) x->_width, ( int ) x->_height );
			if( r )
				rect.intersect( *r );
			alloc( rect.width, rect.height, x->_order, x->_type );

			dst[ 0 ] = 0;
			dst[ 1 ] = 0;
			dst[ 2 ] = 0;
			src[ 0 ] = rect.x;
			src[ 1 ] = rect.y;
			src[ 2 ] = 0;
			crect[ 0 ] = rect.width;
			crect[ 1 ] = rect.height;
			crect[ 2 ] = 1;

			err = _cl->getCLQueue( ).enqueueCopyImage( *clmem->_climage, *_climage, src, dst, crect, NULL, NULL );
			if( err != CL_SUCCESS )
				throw CLException( __PRETTY_FUNCTION__, err );
		}
	}

	uint8_t* ImageAllocatorCL::map( size_t* stride )
	{
		uint8_t* ptr;
		cl_int err;
		cl::size_t<3> src;
		cl::size_t<3> rect;

		src[ 0 ] = 0;
		src[ 1 ] = 0;
		src[ 2 ] = 0;
		rect[ 0 ] = _width;
		rect[ 1 ] = _height;
		rect[ 2 ] = 1;


		ptr = ( uint8_t* )_cl->getCLQueue( ).enqueueMapImage( *_climage, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, src, rect, stride, NULL, NULL, NULL, &err );
		if( err != CL_SUCCESS )
			throw CLException( __PRETTY_FUNCTION__, err );
		return ptr;
	}

	const uint8_t* ImageAllocatorCL::map( size_t* stride ) const
	{
		uint8_t* ptr;
		cl_int err;
		cl::size_t<3> src;
		cl::size_t<3> rect;

		src[ 0 ] = 0;
		src[ 1 ] = 0;
		src[ 2 ] = 0;
		rect[ 0 ] = _width;
		rect[ 1 ] = _height;
		rect[ 2 ] = 1;

		ptr = ( uint8_t* )_cl->getCLQueue( ).enqueueMapImage( *_climage, CL_TRUE, CL_MAP_READ, src, rect, stride, NULL, NULL, NULL, &err );
		if( err != CL_SUCCESS )
			throw CLException( __PRETTY_FUNCTION__, err );
		return ptr;
	}

	void ImageAllocatorCL::unmap( const uint8_t* ptr ) const
	{
		cl_int err;
		err = _cl->getCLQueue( ).enqueueUnmapMemObject( *_climage, ( void* ) ptr, NULL, NULL );
		if( err != CL_SUCCESS )
			throw CLException( __PRETTY_FUNCTION__, err );
	}


	::cl::ImageFormat ImageAllocatorCL::getCLFormat( IOrder order, IType type )
	{
		cl_channel_order clorder;
		cl_channel_type  cltype;

		switch( order.id ) {
			case ICHANNELORDER_GRAY: clorder = CL_INTENSITY; break;
			case ICHANNELORDER_GRAYALPHA: clorder =CL_RA; break;
			case ICHANNELORDER_RGBA:	clorder = CL_RGBA; break;
			case ICHANNELORDER_BGRA:	clorder = CL_BGRA; break;
		}

		switch( type.id ) {
			case ICHANNELTYPE_UBYTE: cltype = CL_UNORM_INT8; break;
			case ICHANNELTYPE_FLOAT: cltype = CL_FLOAT; break;
		}

		return cl::ImageFormat( clorder, cltype );
	}
}
