#include <cvt/cl/CLImage.h>

namespace cvt {

	CLImage::CLImage( size_t w, size_t h, ImageChannelOrder order, ImageChannelType type ) : IFilterParameter( IFILTERPARAMETER_IMAGE ),
																							 _order( order ), _type( type ), _width( w ), _height( h )
	{
		if( _width == 0 || _height == 0 ) {
			_climage = cl::Image2D();
		} else {
			cl_int err;
			_cl = CLContext::getCurrent();
			_climage = cl::Image2D( _cl->getCLContext( ), CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, getCLFormat( _order, _type ), _width, _height, 0, NULL, &err );
			if( err != CL_SUCCESS )
				throw CLException( __PRETTY_FUNCTION__, err );
		}
	}

	CLImage::CLImage( const CLImage& i ) : IFilterParameter( IFILTERPARAMETER_IMAGE )
	{
		copy( i );
	}

	CLImage::CLImage( const Image& i ) : IFilterParameter( IFILTERPARAMETER_IMAGE )
	{
		cl_int err;

		_cl = CLContext::getCurrent();
		_width = i.width();
		_height = i.height();
		_order = i.order();
		_type = i.type();

		_climage = cl::Image2D( _cl->getCLContext( ), CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, getCLFormat( _order, _type ), _width, _height, 0, NULL, &err );
		if( err != CL_SUCCESS )
			throw CLException( __PRETTY_FUNCTION__, err );

		writeImage( i );
	}

	CLImage::~CLImage()
	{

	}

	void CLImage::reallocate( const CLImage& i )
	{
		cl_int err;

		if( i.width() != _width || i.height() != _height ||
		   i.order() != _order || i.type() != _type  ) {
			_cl = i._cl;
			_width = i._width;
			_height = i._height;
			_order = i._order;
			_type = i._type;
			_climage = cl::Image2D( _cl->getCLContext( ), CL_MEM_READ_WRITE, getCLFormat( _order, _type ), _width, _height, 0, NULL, &err );

			if( err != CL_SUCCESS )
				throw CLException( __PRETTY_FUNCTION__, err );
		}
	}

	void CLImage::copy( const CLImage& i )
	{
		cl_int err;
		cl::size_t<3> origin;
		cl::size_t<3> region;

		reallocate( i );

		origin[ 0 ] = 0;
		origin[ 1 ] = 0;
		origin[ 2 ] = 0;
		region[ 0 ] = _width;
		region[ 1 ] = _height;
		region[ 2 ] = 1;
		err = _cl->getCLQueue( ).enqueueCopyImage( i._climage, _climage, origin, origin, region );
		if( err != CL_SUCCESS )
			throw CLException( __PRETTY_FUNCTION__, err );
	}

	void CLImage::readImage( Image& i )
	{
		i.reallocate( _width, _height, _order, _type );
		readData( i.data(), i.stride() );
	}

	void CLImage::readData( void* data, size_t stride )
	{
		cl_int err;
		cl::size_t<3> origin;
		cl::size_t<3> region;

		origin[ 0 ] = 0;
		origin[ 1 ] = 0;
		origin[ 2 ] = 0;
		region[ 0 ] = _width;
		region[ 1 ] = _height;
		region[ 2 ] = 1;

		err = _cl->getCLQueue( ).enqueueReadImage( _climage, CL_TRUE, origin, region, stride, 0, data );
		if( err != CL_SUCCESS )
			throw CLException( __PRETTY_FUNCTION__, err );
	}

	void CLImage::writeImage( const Image& i ) const
	{
		if( i.width() != _width || i.height() != _height ||
		   i.order() != _order || i.type() != _type  )
			throw CVTException("Image mismatch!");
		writeData( i.data(), i.stride() );
	}

	void CLImage::writeData( void const* data, size_t stride ) const
	{
		cl_int err;
		cl::size_t<3> origin;
		cl::size_t<3> region;

		origin[ 0 ] = 0;
		origin[ 1 ] = 0;
		origin[ 2 ] = 0;
		region[ 0 ] = _width;
		region[ 1 ] = _height;
		region[ 2 ] = 1;

		err = _cl->getCLQueue( ).enqueueWriteImage( _climage, CL_TRUE, origin, region, stride, 0, data );
		if( err != CL_SUCCESS )
			throw CLException( __PRETTY_FUNCTION__, err );
	}

	::cl::ImageFormat CLImage::getCLFormat( ImageChannelOrder order, ImageChannelType type )
	{
		cl_channel_order clorder;
		cl_channel_type  cltype;

		switch( order ) {
			case CVT_GRAY: clorder = CL_LUMINANCE; break;
			case CVT_GRAYALPHA: clorder =CL_RA; break;
			case CVT_RGBA:	clorder = CL_RGBA; break;
			case CVT_BGRA:	clorder = CL_BGRA; break;
		}

		switch( type ) {
			case CVT_UBYTE: cltype = CL_UNORM_INT8; break;
			case CVT_FLOAT: cltype = CL_FLOAT; break;
		}

		return cl::ImageFormat( clorder, cltype );
	}

}


