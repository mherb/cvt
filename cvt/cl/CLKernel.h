#ifndef CVT_CLKERNEL_H
#define CVT_CLKERNEL_H

#include <cvt/cl/CLObject.h>
#include <cvt/cl/CLUtil.h>
#include <cvt/cl/CLException.h>
#include <cvt/cl/CLLocalSpace.h>
#include <cvt/cl/CLProgram.h>
#include <cvt/cl/CLBuffer.h>
#include <cvt/cl/CLImage2D.h>
#include <cvt/cl/CLNDRange.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/ImageAllocatorCL.h>
#include <string>
#include <iostream>

namespace cvt {

	class CLKernel : public CLObject<cl_kernel>
	{
		public:
			CLKernel( cl_kernel k = NULL );
			CLKernel( const CLProgram& prog, const char* name );
			CLKernel( const CLProgram& prog, const String& name );
			CLKernel( const String& source, const String& name );

			void setArg( cl_uint index, CLBuffer& arg );
			void setArg( cl_uint index, CLImage2D& arg );
			void setArg( cl_uint index, Image& arg );
			void setArg( cl_uint index, const CLBuffer& arg );
			void setArg( cl_uint index, const CLImage2D& arg );
			void setArg( cl_uint index, const Image& arg );
			template<typename T>
			void setArg( cl_uint index, T arg );
			void setArg( cl_uint index, size_t size, void* arg );

			CLNDRange bestLocalRange1d( const CLNDRange& global ) const;
			CLNDRange bestLocalRange2d( const CLNDRange& global ) const;
			void run( const CLNDRange& global, const CLNDRange& local ) const;


			CLUTIL_GETINFOSTRING( functionName, CL_KERNEL_FUNCTION_NAME, _object, ::clGetKernelInfo  )

			CLUTIL_GETINFOTYPE( numArgs, CL_KERNEL_NUM_ARGS, cl_uint, _object, ::clGetKernelInfo );
	};

	inline CLKernel::CLKernel( cl_kernel k ) : CLObject<cl_kernel>( k )
	{
	}

	inline CLKernel::CLKernel( const CLProgram& prog, const char* name )
	{
		cl_int err;
		_object = ::clCreateKernel( prog, name, &err );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	inline CLKernel::CLKernel( const CLProgram& prog, const String& name )
	{
		cl_int err;
		_object = ::clCreateKernel( prog, name.c_str(), &err );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}


	inline CLKernel::CLKernel( const String& source, const String& name )
	{
		CLProgram prog( *CL::defaultContext(), source );
		if( ! prog.build( *CL::defaultDevice() ) ) {
			String log;
			prog.buildLog( *CL::defaultDevice(), log );
			throw CVTException( log.c_str() );
		}

		cl_int err;
		_object = ::clCreateKernel( prog, name.c_str(), &err );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	inline CLNDRange CLKernel::bestLocalRange1d( const CLNDRange& global ) const
	{
		cl_int err;
		size_t devmaxwg = CL::defaultDevice()->maxWorkGroupSize();
		size_t kernmaxwg;
		CLNDRange ranges;

		ranges = CL::defaultDevice()->maxWorkItemSizes();

		err = ::clGetKernelWorkGroupInfo( _object, *CL::defaultDevice(), CL_KERNEL_WORK_GROUP_SIZE, sizeof( size_t ), &kernmaxwg, NULL );
		if( err != CL_SUCCESS )
			throw CLException( err );

		// FUCK - MacBook OpenCL is buggy - returns 1024 for max work item size, although it really is 1 ...
		//std::cout << "DEVICE MAX WG: " << devmaxwg << std::endl;
		//std::cout << "KERNEL MAX WG: " << kernmaxwg << std::endl;
		//std::cout << "MAX WG SIZES: " << ranges << std::endl;
		return Math::gcd<size_t>( *global.range(), Math::min( Math::min( devmaxwg, kernmaxwg ), *ranges.range() ) );
	}

	inline CLNDRange CLKernel::bestLocalRange2d( const CLNDRange& global ) const
	{
		return CLNDRange( 1, 1 );
	}

	inline void CLKernel::setArg( cl_uint index, CLBuffer& buf )
	{
		cl_int err;
		cl_mem mem = ( cl_mem ) buf;
		err = ::clSetKernelArg( _object, index, sizeof( cl_mem ), &mem );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	inline void CLKernel::setArg( cl_uint index, const CLBuffer& buf )
	{
		cl_int err;
		cl_mem mem = ( cl_mem ) buf;
		err = ::clSetKernelArg( _object, index, sizeof( cl_mem ), &mem );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	inline void CLKernel::setArg( cl_uint index, CLImage2D& img )
	{
		cl_int err;
		cl_mem mem = ( cl_mem ) img;
		err = ::clSetKernelArg( _object, index, sizeof( cl_mem ), &mem );

		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	inline void CLKernel::setArg( cl_uint index, const CLImage2D& img )
	{
		cl_int err;
		cl_mem mem = ( cl_mem ) img;
		err = ::clSetKernelArg( _object, index, sizeof( cl_mem ), &mem );

		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	inline void CLKernel::setArg( cl_uint index, Image& img )
	{
		ImageAllocatorCL* memcl = dynamic_cast<ImageAllocatorCL*>( img._mem );
		if( !memcl )
			throw CVTException( "Image invalid for OpenCL" );
		setArg( index, *memcl->_climage );
	}

	inline void CLKernel::setArg( cl_uint index, const Image& img )
	{
		ImageAllocatorCL* memcl = dynamic_cast<ImageAllocatorCL*>( img._mem );
		if( !memcl )
			throw CVTException( "Image invalid for OpenCL" );
		setArg( index, *memcl->_climage );
	}


	template<typename T>
	inline void CLKernel::setArg( cl_uint index, T arg )
	{
		cl_int err;
		err = ::clSetKernelArg( _object, index, sizeof( arg ), &arg );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	template<>
	inline void CLKernel::setArg<CLLocalSpace>( cl_uint index, CLLocalSpace s )
	{
		cl_int err;
		err = ::clSetKernelArg( _object, index, s.size, NULL );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}


	inline void CLKernel::setArg( cl_uint index, size_t size, void* arg )
	{
		cl_int err;
		err = ::clSetKernelArg( _object, index, size, arg );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	inline std::ostream& operator<<( std::ostream& out, const CLKernel& kern )
	{
		String name;
		kern.functionName( name );
		out << "Kernel:\n\tName: " << name;
		out << "\n\tArgs: " << kern.numArgs();
		return out;
	}
}

#endif
