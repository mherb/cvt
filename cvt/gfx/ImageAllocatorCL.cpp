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

	void ImageAllocatorCL::alloc( size_t width, size_t height, const IFormat & format )
	{
		cl_int err;

		if( _climage )
			delete _climage;
		_width = width;
		_height = height;
		_format = format;
		_cl = CLContext::getCurrent();
		_climage = new cl::Image2D( _cl->getCLContext( ), CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, getCLFormat( _format ), _width, _height, 0, NULL, &err );
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
			cl_int err;
			cl::size_t<3> src;
			cl::size_t<3> dst;
			cl::size_t<3> crect;
			Recti rect( 0, 0, ( int ) x->_width, ( int ) x->_height );
			if( r )
				rect.intersect( *r );
			alloc( rect.width, rect.height, x->_format );

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


	::cl::ImageFormat ImageAllocatorCL::getCLFormat( const IFormat & format )
	{
		cl_channel_order clorder;
		cl_channel_type  cltype;

		switch ( format.formatID ) {
			case IFORMAT_GRAY_UINT8:		clorder = CL_INTENSITY; cltype = CL_UNORM_INT8; break;
			case IFORMAT_GRAY_UINT16:		clorder = CL_INTENSITY; cltype = CL_UNORM_INT16; break;
			case IFORMAT_GRAY_INT16:		clorder = CL_INTENSITY; cltype = CL_SNORM_INT16; break;
			case IFORMAT_GRAY_FLOAT:		clorder = CL_INTENSITY;	cltype = CL_FLOAT; break;

			case IFORMAT_GRAYALPHA_UINT8:	clorder = CL_RA; cltype = CL_UNORM_INT8; break;
			case IFORMAT_GRAYALPHA_UINT16:	clorder = CL_RA; cltype = CL_UNORM_INT16; break;
			case IFORMAT_GRAYALPHA_INT16:	clorder = CL_RA; cltype = CL_SNORM_INT16; break;
			case IFORMAT_GRAYALPHA_FLOAT:	clorder = CL_RA; cltype = CL_FLOAT; break;

			case IFORMAT_RGBA_UINT8:		clorder = CL_RGBA; cltype = CL_UNORM_INT8; break;
			case IFORMAT_RGBA_UINT16:		clorder = CL_RGBA; cltype = CL_UNORM_INT16; break;
			case IFORMAT_RGBA_INT16:		clorder = CL_RGBA; cltype = CL_SNORM_INT16; break;
			case IFORMAT_RGBA_FLOAT:		clorder = CL_RGBA; cltype = CL_FLOAT; break;

			case IFORMAT_BGRA_UINT8:		clorder = CL_BGRA; cltype = CL_UNORM_INT8; break;
			case IFORMAT_BGRA_UINT16:		clorder = CL_BGRA; cltype = CL_UNORM_INT16; break;
			case IFORMAT_BGRA_INT16:		clorder = CL_BGRA; cltype = CL_SNORM_INT16; break;
			case IFORMAT_BGRA_FLOAT:		clorder = CL_BGRA; cltype = CL_FLOAT; break;

			case IFORMAT_BAYER_RGGB_UINT8:	clorder = CL_INTENSITY; cltype = CL_UNORM_INT8; break;

			case IFORMAT_YUYV_UINT8:		clorder = CL_RA; cltype = CL_UNORM_INT8; break;
			case IFORMAT_UYVY_UINT8:		clorder = CL_RA; cltype = CL_UNORM_INT8; break;
			default:
				throw CVTException( "No equivalent CL format found" );
				break;
		}

		return cl::ImageFormat( clorder, cltype );
	}
}
