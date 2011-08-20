#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLDevice.h>
#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLContext.h>

namespace cvt {
	bool			CL::_glsharing = false;
	CLContext*	    CL::_ctx = NULL;
	CLDevice*	    CL::_device = NULL;
	CLCommandQueue* CL::_queue = NULL;

	bool CL::init( cl_device_id id, cl_context_properties* props )
	{
		cl_int err;

		cl_context ctx = ::clCreateContext( props, 1,  ( cl_device_id* ) &id, NULL, NULL, &err );
		if( err ) {
			std::cout << CLException::CLErrorToString( err ) << std::endl;
			return false;
		}

		_ctx = new CLContext( ctx );
		// release ctx since it is retained by _ctx
		::clReleaseContext( ctx );
		_device = new CLDevice( id );
		std::cout << _ctx->refCount() << std::endl;
		return true;
	}
}
