#include <CLContext.h>
#include <CLImage2D.h>

namespace cvt {
	CLImage2D::CLImage2D( const CLContext& context, size_t width, size_t height, const CLImageFormat& format,
						  cl_mem_flags flags, size_t stride, void* host_ptr)
	{
		cl_int err;
		_object = ::clCreateImage2D( context, flags, ( cl_image_format* ) &format, width, height, stride, host_ptr, &err );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}


}
