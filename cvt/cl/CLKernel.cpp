/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
