/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_CLEVENT_H
#define CVT_CLEVENT_H

#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLObject.h>
#include <cvt/cl/CLUtil.h>

namespace cvt {
	class CLCommandQueue;
	class CLContext;

	/**
	  \ingroup CL
	 */
	class CLEvent : public CLObject<cl_event>
	{
		public:
			CLEvent( cl_event e = NULL ) : CLObject<cl_event>( e ) {};
			CLEvent( const CLEvent& e ) : CLObject<cl_event>( e._object ) {};

			CLUTIL_GETINFOTYPE( type, CL_EVENT_COMMAND_TYPE, cl_command_type, _object, ::clGetEventInfo )
			CLUTIL_GETINFOTYPE( status, CL_EVENT_COMMAND_EXECUTION_STATUS, cl_int, _object, ::clGetEventInfo )

			void wait() const;

			static void waitEvents( const std::vector<CLEvent>& waitevents );

			CLCommandQueue queue() const;
			CLContext context() const;

		private:
			CLUTIL_GETINFOTYPE( _queue, CL_EVENT_COMMAND_QUEUE, cl_command_queue, _object, ::clGetEventInfo )
			// OpenCL 1.1
			// CLUTIL_GETINFOTYPE( _context, CL_EVENT_CONTEXT, cl_context, _object, ::clGetEventInfo )
	};

	inline void CLEvent::wait() const
	{
		cl_int err;
		err = ::clWaitForEvents( 1, ( cl_event* ) this );
		if( err != CL_SUCCESS )
			throw CLException( __PRETTY_FUNCTION__, err );
	}

	inline void CLEvent::waitEvents( const std::vector<CLEvent>& waitevents )
	{
		cl_int err;
		err = ::clWaitForEvents( waitevents.size(), ( cl_event* )  ( const cl_event* ) &waitevents[0] );
		if( err != CL_SUCCESS )
			throw CLException( __PRETTY_FUNCTION__, err );
	}
}

#endif
