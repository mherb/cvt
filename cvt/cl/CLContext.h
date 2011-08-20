#ifndef CVT_CLCONTEXT_H
#define CVT_CLCONTEXT_H

#include <cvt/cl/CLPlatform.h>
#include <cvt/cl/CLObject.h>
#include <cvt/cl/CLCommandQueue.h>
#include <cvt/cl/CLImageFormat.h>

namespace cvt {
	class CLContext : public CLObject<cl_context> {
		public:
			CLContext( cl_context ctx = NULL );
			CLContext( const CLContext& ctx );
			CLContext( const CLPlatform& platform, const std::vector<CLDevice>& devices, bool cerrlog = false );
			CLContext( const CLPlatform& platform, const CLDevice& device, bool cerrlog = false );
			CLContext( const CLPlatform& platform, cl_device_type type, bool cerrlog = false );
			~CLContext();

			void devices( std::vector<CLDevice>& devices ) const;

//			CLCommandQueue defaultCommandQueue() const;
			CLDevice defaultDevice() const;

			CLUTIL_GETINFOTYPE( refCount, CL_CONTEXT_REFERENCE_COUNT, cl_uint, _object, ::clGetContextInfo )

			void supportedImage2DFormats( std::vector<CLImageFormat>& formats ) const;

		private:
			void initDefault();
			static void cerrlog( const char* errinfo, const void* pinfo, size_t pinfosize, void* );

//			CLCommandQueue _queue;
	};

}

#endif
