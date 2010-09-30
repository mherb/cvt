#ifndef CLKERNEL_H
#define CLKERNEL_H

#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLException.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/IFilterParameter.h>
#include <string>
#include <vector>


namespace cvt {

	class CLKernel {
		public:
			CLKernel() : _cl( 0 ) {};
			void setArg( size_t n, IFilterParameter* p );
			void setArg( size_t n, size_t size );
			void build( const char* name, const char* src, size_t len, std::string& buildinfo );
			void run( const cl::NDRange& offset, const cl::NDRange& global, const cl::NDRange& local, std::vector<cl::Event>* events = NULL, cl::Event* event = NULL );
			void run( const Image& img, const cl::NDRange& local, std::vector<cl::Event>* events = NULL, cl::Event* event = NULL );
			size_t workGroupSize( ) const;
			std::vector<char*>* getBinaries( );

		private:
			CLKernel( const CLKernel& ) : _cl( 0 ) {};

			const char* _name;
			::cl::Kernel _kernel;
			::cl::Program _prog;
			CLContext* _cl;
	};

}


#endif
