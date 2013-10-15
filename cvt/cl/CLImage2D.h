#ifndef CVT_CLIAMGE2D_H
#define CVT_CLIAMGE2D_H

#include <cvt/cl/CLObject.h>
#include <cvt/cl/CLUtil.h>
#include <cvt/cl/CLException.h>
#include <cvt/cl/CLImageFormat.h>

#include <cvt/gfx/Image.h>

namespace cvt {
	class CLContext;

	/**
	  \ingroup CL
	 */
	class CLImage2D : public CLMemory
	{
		public:
			CLImage2D( const CLContext& context, size_t width, size_t height, const CLImageFormat& format,
					  cl_mem_flags flags = CL_MEM_READ_WRITE, size_t stride = 0, void* host_ptr = NULL );
			CLImage2D( size_t width, size_t height, const CLImageFormat& format, cl_mem_flags flags = CL_MEM_READ_WRITE );

			CLImage2D( const CLImage2D& img, const Recti* r = NULL, cl_mem_flags flags = CL_MEM_READ_WRITE );

			//CLImage2D( const Image& img );

			void*		map( size_t* stride );
			const void* map( size_t* stride ) const;
			void		unmap( const void* ptr ) const;

			CLUTIL_GETINFOTYPE( memType, CL_MEM_TYPE, cl_mem_object_type, _object, ::clGetMemObjectInfo )
			CLUTIL_GETINFOTYPE( memFlags, CL_MEM_FLAGS, cl_mem_flags, _object, ::clGetMemObjectInfo )
			CLUTIL_GETINFOTYPE( hostPtr, CL_MEM_HOST_PTR, void*, _object, ::clGetMemObjectInfo )
			CLUTIL_GETINFOTYPE( mapCount, CL_MEM_MAP_COUNT, cl_uint, _object, ::clGetMemObjectInfo )

			// Missing
			// - CL_IMAGE_ROW_PITCH  gives stride in bytes
			// - CL_IMAGE_WIDTH	 width
			// - CL_IMAGE_HEIGHT height
			// - CL_IMAGE_FORMAT CLImageFormat
			// - CL_IMAGE_ELEMENT_SIZE bpp
		private:

			size_t _width;
			size_t _height;
			CLImageFormat _format;
	};


}

#endif
