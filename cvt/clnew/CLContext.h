#ifndef CVT_CLCONTEXT_H
#define CVT_CLCONTEXT_H

#include <CLPlatform.h>
#include <CLObject.h>
#include <CLCommandQueue.h>
#include <CLImageFormat.h>

namespace cvt {
	class CLContext : public CLObject<cl_context> {
		public:
			CLContext( cl_context ctx = NULL );
			CLContext( const CLContext& ctx );
			CLContext( const CLPlatform& platform, const std::vector<CLDevice>& devices, bool cerrlog = false );
			CLContext( const CLPlatform& platform, const CLDevice& device, bool cerrlog = false );
			CLContext( const CLPlatform& platform, cl_device_type type, bool cerrlog = false );

			void devices( std::vector<CLDevice>& devices ) const;

//			CLCommandQueue defaultCommandQueue() const;
			CLDevice defaultDevice() const;

			void supportedImage2DFormats( std::vector<CLImageFormat>& formats ) const;

		private:
			void initDefault();
			static void cerrlog( const char* errinfo, const void* pinfo, size_t pinfosize, void* );

//			CLCommandQueue _queue;
	};

}

#endif
