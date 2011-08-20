#include <cvt/cl/OpenCL.h>

namespace cvt {
	bool			CL::_glsharing = false;
	CLContext*	    CL::_ctx = NULL;
	CLDevice*	    CL::_device = NULL;
	CLCommandQueue* CL::_queue = NULL;

}
