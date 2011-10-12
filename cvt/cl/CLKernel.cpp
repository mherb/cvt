#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLCommandQueue.h>

namespace cvt {

	void CLKernel::run( const CLNDRange& global, const CLNDRange& local ) const
	{
		CL::defaultQueue()->enqueueNDRangeKernel( *this, global, local );
	}
}
