#ifndef CVT_CLIAMGE2D_H
#define CVT_CLIAMGE2D_H

#include <CLObject.h>
#include <CLUtil.h>
#include <CLException.h>
#include <CLImageFormat.h>

namespace cvt {
	class CLContext;

	class CLImage2D : public CLMemory
	{
		public:
			CLImage2D( const CLContext& context, size_t width, size_t height, const CLImageFormat& format,
					   cl_mem_flags flags = CL_MEM_READ_WRITE, size_t stride = 0, void* host_ptr = NULL );

		private:
			CLImage2D( const CLImage2D& img );
	};

}

#endif
