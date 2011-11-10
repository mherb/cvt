#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLCommandQueue.h>

namespace cvt {

	void CLKernel::run( const CLNDRange& global, const CLNDRange& local, const CLNDRange& offset ) const
	{
		CL::defaultQueue()->enqueueNDRangeKernel( *this, global, local, offset );
	}

	void CLKernel::runWait( const CLNDRange& global, const CLNDRange& local, const CLNDRange& offset ) const
	{
		CLEvent e;
		CL::defaultQueue()->enqueueNDRangeKernel( *this, global, local, offset, NULL, &e );
		e.wait();
	}

}
