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

			CLCommandQueue queue() const;
			CLContext context() const;

		private:
			CLUTIL_GETINFOTYPE( _queue, CL_EVENT_COMMAND_QUEUE, cl_command_queue, _object, ::clGetEventInfo )
			// OpenCL 1.1
			// CLUTIL_GETINFOTYPE( _context, CL_EVENT_CONTEXT, cl_context, _object, ::clGetEventInfo )
	};
}

#endif
