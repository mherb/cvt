/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
