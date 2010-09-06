#include <cvt/cl/CLImage.h>

namespace cvt {
	namespace cl {

		CLImage::CLImage( CLManager* cl )
			: CLParameter( CL_PARAM_IMAGE ), format( CL_RGBA, CL_UNORM_INT8 ), width( 0 ), height( 0 ), mapptr( 0 ), mapstride( 0 )
		{
			this->cl = cl;
			climage = ::cl::Image2D( );
			clmem = climage( );
		}

		CLImage::CLImage( CLManager* cl, ::cl::ImageFormat f, size_t w, size_t h, void* dataptr, size_t stride )
			: CLParameter( CL_PARAM_IMAGE ), format( f ), width( w ), height( h ), mapptr( 0 ), mapstride( 0 )
		{
			cl_int err;

			this->cl = cl;
			climage = ::cl::Image2D( cl->getCLContext( ), CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, format, width, height, 0, NULL, &err );
			clmem = climage( );

			if( err != CL_SUCCESS )
				throw CLException( __PRETTY_FUNCTION__, err );

			if( dataptr ) {
				::cl::size_t<3> origin;
				::cl::size_t<3> region;

				origin[ 0 ] = 0;
				origin[ 1 ] = 0;
				origin[ 2 ] = 0;
				region[ 0 ] = width;
				region[ 1 ] = height;
				region[ 2 ] = 1;

				err = cl->getCLQueue( ).enqueueWriteImage( climage, CL_TRUE, origin, region, stride, 0, dataptr );
				if( err != CL_SUCCESS )
					throw CLException( __PRETTY_FUNCTION__, err );
			}
		}

		std::pair<size_t, void*> CLImage::getCLArgument( )
		{
			return std::make_pair<size_t, void*>( sizeof( cl_mem ), static_cast<void*>( &clmem ) );
		}

		::cl::NDRange CLImage::getCLNDRange( ) const
		{
			return ::cl::NDRange( width, height );
		}

		void CLImage::write( void* data, size_t stride )
		{
			cl_int err;
			::cl::size_t<3> origin;
			::cl::size_t<3> region;

			origin[ 0 ] = 0;
			origin[ 1 ] = 0;
			origin[ 2 ] = 0;
			region[ 0 ] = width;
			region[ 1 ] = height;
			region[ 2 ] = 1;

			err = cl->getCLQueue( ).enqueueWriteImage( climage, CL_FALSE, origin, region, stride, 0, data );
			if( err != CL_SUCCESS )
				throw CLException( __PRETTY_FUNCTION__, err );
		}

		void CLImage::read( void* data, size_t stride ) const
		{
			cl_int err;
			::cl::size_t<3> origin;
			::cl::size_t<3> region;

			origin[ 0 ] = 0;
			origin[ 1 ] = 0;
			origin[ 2 ] = 0;
			region[ 0 ] = width;
			region[ 1 ] = height;
			region[ 2 ] = 1;

			err = cl->getCLQueue( ).enqueueReadImage( climage, CL_TRUE, origin, region, stride, 0, data );
			if( err != CL_SUCCESS )
				throw CLException( __PRETTY_FUNCTION__, err );
		}

		void CLImage::copy( const CLImage& src )
		{
			cl_int err;
			::cl::size_t<3> origin;
			::cl::size_t<3> region;

			if( width != src.getWidth( ) || height != src.getHeight( ) || src.climage( ) == climage( ) )
				return;

			origin[ 0 ] = 0;
			origin[ 1 ] = 0;
			origin[ 2 ] = 0;
			region[ 0 ] = width;
			region[ 1 ] = height;
			region[ 2 ] = 1;
			err = cl->getCLQueue( ).enqueueCopyImage( src.climage, climage, origin, origin, region );
			if( err != CL_SUCCESS )
				throw CLException( __PRETTY_FUNCTION__, err );
		}

		void* CLImage::map( size_t& stride )
		{
			cl_int err;
			::cl::size_t<3> origin;
			::cl::size_t<3> region;

			if( mapptr )
				return mapptr;
			origin[ 0 ] = 0;
			origin[ 1 ] = 0;
			origin[ 2 ] = 0;
			region[ 0 ] = width;
			region[ 1 ] = height;
			region[ 2 ] = 1;

			mapptr = cl->getCLQueue( ).enqueueMapImage( climage, CL_TRUE, CL_MEM_READ_WRITE, origin, region, &mapstride, NULL, NULL, NULL, &err );
			stride = mapstride;
			if( err != CL_SUCCESS )
				throw CLException( __PRETTY_FUNCTION__, err );

			return mapptr;
		}

		void CLImage::unmap( )
		{
			cl_int err;
			if( !mapptr )
				return;

			err = cl->getCLQueue( ).enqueueUnmapMemObject( climage, mapptr );
			mapptr = 0;
			if( err != CL_SUCCESS )
				throw CLException( __PRETTY_FUNCTION__, err );
		}

		void CLImage::syncMapRead( )
		{
			if( !mapptr )
				return;
			read( mapptr, mapstride );
		}

		void CLImage::syncMapWrite( )
		{
			if( !mapptr )
				return;
			write( mapptr, mapstride );
		}

		CLImage::CLImage( ) : CLParameter( CL_PARAM_IMAGE )
		{}

		CLImage::~CLImage( )
		{
			if( mapptr )
				unmap( );
		}

	}
}
