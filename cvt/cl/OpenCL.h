#ifndef CVT_OPENCL_H
#define CVT_OPENCL_H

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/opencl.h>
#include <OpenCL/cl_gl.h>
#else
#include <CL/opencl.h>
#include <CL/cl_gl.h>
#endif

namespace cvt {
	class Application;
	class ApplicationX11;
	class CLContext;
	class CLCommandQueue;
	class CLDevice;
	class GLContext;

	/**
	  \ingroup CL
	  \defgroup CL OpenCL
	 */
	class CL {
		friend class Application;
		friend class ApplicationX11;
		friend class ApplicationOSX;

		public:
			static CLContext* defaultContext() { return _ctx; }
			static CLCommandQueue* defaultQueue() { return _queue; }
			static CLDevice* defaultDevice() { return _device; }
			static bool	defaultGLsharing() { return _glsharing; }
			static bool setDefaultDevice( const CLDevice& dev );
		private:
			static bool init( const GLContext* glctx );
			static void cleanup();

			// default context, device and queue
			static bool			   _glsharing;
			static CLContext*	   _ctx;
			static CLDevice*	   _device;
			static CLCommandQueue* _queue;
	};
}

#endif
