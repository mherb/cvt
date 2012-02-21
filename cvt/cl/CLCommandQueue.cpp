/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/cl/CLCommandQueue.h>
#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLDevice.h>

namespace cvt {
	CLCommandQueue::CLCommandQueue( cl_command_queue q ) : CLObject<cl_command_queue>( q )
	{
	}

	CLCommandQueue::CLCommandQueue( const CLCommandQueue& q ) : CLObject<cl_command_queue>( q._object )
	{
	}

	CLCommandQueue::CLCommandQueue( const CLContext& context, const CLDevice& device, cl_command_queue_properties props )
	{
		cl_int err;
		_object = ::clCreateCommandQueue( context, device, props, &err );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	/**
	  The context associated with the command-queue.
	 */
	CLContext CLCommandQueue::context() const
	{
		return CLContext( _context() );
	}

	/**
	  The device associated with the command-queue.
	 */
	CLDevice CLCommandQueue::device() const
	{
		return CLDevice( _device() );
	}

}
