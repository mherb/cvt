#include <cvt/cl/CLEvent.h>
#include <cvt/cl/CLCommandQueue.h>
#include <cvt/cl/CLContext.h>

namespace cvt {

	CLCommandQueue CLEvent::queue() const
	{
		return CLCommandQueue( _queue() );
	}

	/* OpenCL 1.1
	CLContext CLEvent::context() const
	{
		return CLContext( _context() );
	}*/

}
