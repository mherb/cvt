#ifndef CVT_CLBUFFER_H
#define CVT_CLBUFFER_H

#include <CLObject.h>
#include <CLUtil.h>

namespace cvt {
	class CLContext;

	class CLBuffer : public CLMemory
	{
		public:
			CLBuffer( const CLContext& context, size_t size, cl_mem_flags flags = CL_MEM_READ_WRITE, void* host_ptr = NULL );

			CLUTIL_GETINFOTYPE( memType, CL_MEM_TYPE, cl_mem_object_type, _object, ::clGetMemObjectInfo )
			CLUTIL_GETINFOTYPE( memFlags, CL_MEM_FLAGS, cl_mem_flags, _object, ::clGetMemObjectInfo )
			CLUTIL_GETINFOTYPE( hostPtr, CL_MEM_HOST_PTR, void*, _object, ::clGetMemObjectInfo )
			CLUTIL_GETINFOTYPE( size, CL_MEM_SIZE, size_t, _object, ::clGetMemObjectInfo )

			CLContext context() const;

		private:
			CLBuffer( cl_mem mem ) : CLMemory( mem ) {};
			CLBuffer( const CLBuffer& buf ) : CLMemory( buf._object ) {};

			CLUTIL_GETINFOTYPE( memContext, CL_MEM_CONTEXT, cl_context, _object, ::clGetMemObjectInfo )

	};

}

#endif
