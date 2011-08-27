#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLException.h>
#include <cvt/cl/CLCommandQueue.h>
#include <iostream>

namespace cvt {

	CLContext::CLContext( cl_context ctx ) : CLObject<cl_context>( ctx )
	{
	}

	CLContext::CLContext( const CLContext& ctx ) : CLObject<cl_context>( ctx._object )
	{
	}

	CLContext::~CLContext()
	{
	}


	CLContext::CLContext( const CLPlatform& platform, const std::vector<CLDevice>& devices, bool cerrlog )
	{
		cl_int err;
		cl_context_properties props[ 3 ];
		props[ 0 ] = CL_CONTEXT_PLATFORM;
		props[ 1 ] = ( cl_context_properties ) platform;
		props[ 2 ] = NULL;

		if( cerrlog )
			_object = ::clCreateContext( props, devices.size(), ( cl_device_id* ) &devices[ 0 ], CLContext::cerrlog, NULL, &err );
		else
			_object = ::clCreateContext( props, devices.size(), ( cl_device_id* ) &devices[ 0 ], NULL, NULL, &err );
		if( err )
			throw CLException( err );
	}

	CLContext::CLContext( const CLPlatform& platform, const CLDevice& device, bool cerrlog )
	{
		cl_int err;
		cl_context_properties props[ 3 ];
		props[ 0 ] = CL_CONTEXT_PLATFORM;
		props[ 1 ] = ( cl_context_properties ) platform;
		props[ 2 ] = NULL;

		if( cerrlog )
			_object = ::clCreateContext( props, 1,  ( cl_device_id* ) &device, CLContext::cerrlog, NULL, &err );
		else
			_object = ::clCreateContext( props, 1,  ( cl_device_id* ) &device, NULL, NULL, &err );
		if( err )
			throw CLException( err );
	}

	CLContext::CLContext( const CLPlatform& platform, cl_device_type type, bool cerrlog )
	{
		cl_int err;
		cl_context_properties props[ 3 ];
		props[ 0 ] = CL_CONTEXT_PLATFORM;
		props[ 1 ] = ( cl_context_properties ) platform;
		props[ 2 ] = NULL;

		if( cerrlog )
			_object = ::clCreateContextFromType( props, type, CLContext::cerrlog, NULL, &err );
		else
			_object = ::clCreateContextFromType( props, type, NULL, NULL, &err );

		if( err )
			throw CLException( err );
	}

	void CLContext::cerrlog( const char* errinfo, const void* , size_t , void* )
	{
		std::cerr << errinfo << std::endl;
	}

	void CLContext::devices( std::vector<CLDevice>& devices ) const
	{
		cl_int err;
		size_t num;
		cl_device_id* ids;

		err = ::clGetContextInfo( _object, CL_CONTEXT_DEVICES, 0, NULL, &num );
		if( err != CL_SUCCESS )
			throw CLException( err );
		ids = ( cl_device_id* ) alloca( num );
		err = ::clGetContextInfo( _object, CL_CONTEXT_DEVICES, num, ids, &num );
		if( err != CL_SUCCESS )
			throw CLException( err );
		devices.assign( ids, ids + num / sizeof( cl_device_id ) );
	}

	void CLContext::supportedImage2DFormats( std::vector<CLImageFormat>& formats ) const
	{
		cl_int err;
		cl_uint size;

		err = ::clGetSupportedImageFormats( _object, CL_MEM_READ_WRITE, CL_MEM_OBJECT_IMAGE2D, 0, NULL, &size );
		if( err != CL_SUCCESS )
			throw CLException( err );
		cl_image_format* fmts = ( cl_image_format* ) alloca( sizeof( cl_image_format ) * size );
		err = ::clGetSupportedImageFormats( _object, CL_MEM_READ_WRITE, CL_MEM_OBJECT_IMAGE2D, size, fmts, &size );
		if( err != CL_SUCCESS )
			throw CLException( err );
		formats.assign( fmts, fmts + size );
	}

}
