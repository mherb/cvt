#ifndef CVT_CLCOMMANDQUEUE_H
#define CVT_CLCOMMANDQUEUE_H

#include <cvt/cl/CLObject.h>
#include <cvt/cl/CLUtil.h>
#include <cvt/cl/CLException.h>
#include <cvt/cl/CLBuffer.h>
#include <cvt/cl/CLEvent.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/cl/CLNDRange.h>
#include <vector>

namespace cvt {
	class CLContext;
	class CLDevice;

	class CLCommandQueue : public CLObject<cl_command_queue>
	{
		public:
			CLCommandQueue( cl_command_queue q = NULL );
			CLCommandQueue( const CLCommandQueue& q );
			CLCommandQueue( const CLContext& context, const CLDevice& device, cl_command_queue_properties props = NULL );

			CLContext context() const;
			CLDevice device() const;
			CLUTIL_GETINFOTYPE( properties, CL_QUEUE_PROPERTIES, cl_command_queue_properties, _object, ::clGetCommandQueueInfo )

			void enqueueReadBuffer( const CLBuffer& buf, void* dst, size_t size, size_t offset = 0, bool block = true, const std::vector<CLEvent>* waitevents = NULL, CLEvent* event = NULL );
			void enqueueWriteBuffer( CLBuffer& buf, const void* src, size_t size, size_t offset = 0, bool block = true, const std::vector<CLEvent>* waitevents = NULL, CLEvent* event = NULL );
			void enqueueCopyBuffer( CLBuffer& dst, const CLBuffer& src, size_t size, size_t dstoffset = 0, size_t srcoffset = 0, const std::vector<CLEvent>* waitevents = NULL, CLEvent* event = NULL );

			void waitEvents( const std::vector<CLEvent>& waitevents );
			void waitEvent( const CLEvent& event );
			void flush();
			void finish();

			void enqueueNDRangeKernel( const CLKernel& kernel, const CLNDRange& global, const CLNDRange& local = CLNDRange(), const CLNDRange& offset = CLNDRange(), const std::vector<CLEvent>* waitevents = NULL, CLEvent* event = NULL );

		private:
			CLUTIL_GETINFOTYPE( queueContext, CL_QUEUE_CONTEXT, cl_context, _object, ::clGetCommandQueueInfo );
			CLUTIL_GETINFOTYPE( queueDevice, CL_QUEUE_DEVICE, cl_device_id, _object, ::clGetCommandQueueInfo );
	};

	inline void CLCommandQueue::enqueueReadBuffer( const CLBuffer& buf, void* dst, size_t size, size_t offset, bool block, const std::vector<CLEvent>* waitevents, CLEvent* event )
	{
		cl_int err;
		err = ::clEnqueueReadBuffer( _object, buf, block, offset, size, dst, waitevents?waitevents->size() : 0, waitevents?( const cl_event* ) &(*waitevents)[0]:NULL, ( cl_event* ) event );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	inline void CLCommandQueue::enqueueWriteBuffer( CLBuffer& buf, const void* src, size_t size, size_t offset, bool block, const std::vector<CLEvent>* waitevents, CLEvent* event )
	{
		cl_int err;
		err = ::clEnqueueWriteBuffer( _object, buf, block, offset, size, src, waitevents?waitevents->size() : 0, waitevents?( const cl_event* ) &(*waitevents)[0]:NULL, ( cl_event* ) event );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	inline void CLCommandQueue::enqueueCopyBuffer( CLBuffer& dst, const CLBuffer& src, size_t size, size_t dstoffset, size_t srcoffset, const std::vector<CLEvent>* waitevents, CLEvent* event )
	{
		cl_int err;
		err = ::clEnqueueCopyBuffer( _object, src, dst, srcoffset, dstoffset, size, waitevents?waitevents->size() : 0, waitevents?( const cl_event* ) &(*waitevents)[0]:NULL, ( cl_event* ) event );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}


	inline void CLCommandQueue::waitEvents( const std::vector<CLEvent>& waitevents )
	{
		cl_int err;
		err = ::clEnqueueWaitForEvents( _object, waitevents.size(), ( const cl_event* ) &waitevents[0] );
		if( err != CL_SUCCESS )
			throw CLException( err );

	}

	inline void CLCommandQueue::waitEvent( const CLEvent& event )
	{
		cl_int err;
		err = ::clEnqueueWaitForEvents( _object, 1, ( const cl_event* ) &event );
		if( err != CL_SUCCESS )
			throw CLException( err );

	}


	inline void CLCommandQueue::flush()
	{
		::clFlush( _object );
	}

	inline void CLCommandQueue::finish()
	{
		::clFinish( _object );
	}

	inline void CLCommandQueue::enqueueNDRangeKernel( const CLKernel& kernel, const CLNDRange& global, const CLNDRange& local, const CLNDRange& offset, const std::vector<CLEvent>* waitevents , CLEvent* event )
	{
		cl_int err;
		err = ::clEnqueueNDRangeKernel( _object, kernel, global.dimension(), offset.dimension()?offset.range():NULL, global.range(), local.dimension()?local.range():NULL, waitevents?waitevents->size() : 0, waitevents?( const cl_event* ) &(*waitevents)[0]:NULL, ( cl_event* ) event );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}
}

#endif
