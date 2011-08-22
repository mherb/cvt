#ifndef CVT_CLIAMGE2D_H
#define CVT_CLIAMGE2D_H

#include <cvt/cl/CLObject.h>
#include <cvt/cl/CLUtil.h>
#include <cvt/cl/CLException.h>
#include <cvt/cl/CLImageFormat.h>

namespace cvt {
	class CLContext;

	class CLImage2D : public CLMemory
	{
		public:
			CLImage2D( const CLContext& context, size_t width, size_t height, const CLImageFormat& format,
					   cl_mem_flags flags = CL_MEM_READ_WRITE, size_t stride = 0, void* host_ptr = NULL );

			// Missing
			// - CL_IMAGE_ROW_PITCH  gives stride in bytes
			// - CL_IMAGE_WIDTH	 width
			// - CL_IMAGE_HEIGHT height
			// - CL_IMAGE_FORMAT CLImageFormat
			// - CL_IMAGE_ELEMENT_SIZE bpp
		private:
			CLImage2D( const CLImage2D& img );
	};

}

#endif
