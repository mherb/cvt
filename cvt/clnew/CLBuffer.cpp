#include <CLBuffer.h>
#include <CLContext.h>

namespace cvt
{
	CLBuffer::CLBuffer( const CLContext& context, size_t size, cl_mem_flags flags , void* host_ptr )
	{
		cl_int err;
		_object = ::clCreateBuffer( context, flags, size, host_ptr, &err );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}


	CLContext CLBuffer::context() const
	{
		return CLContext( memContext() );
	}

}
