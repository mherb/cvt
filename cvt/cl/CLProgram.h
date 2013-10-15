#ifndef CVT_CLPROGRAM_H
#define CVT_CLPROGRAM_H

#include <cvt/cl/CLObject.h>
#include <cvt/cl/CLUtil.h>
#include <cvt/cl/CLException.h>
#include <cvt/cl/CLDevice.h>
#include <vector>

namespace cvt {
	class CLContext;

	class CLProgram : public CLObject<cl_program>
	{
		public:
			CLProgram( cl_program = NULL );
			CLProgram( const CLProgram& p );
			CLProgram( const CLContext& context, const char* prog );
			CLProgram( const CLContext& context, const String& prog );
			bool build( const CLDevice& dev, const char* options = NULL );
			bool build( const std::vector<CLDevice>& devices, const char* options = NULL );

			CLUTIL_GETINFOSTRING( source, CL_PROGRAM_SOURCE, _object, ::clGetProgramInfo )
			void buildLog( const CLDevice& dev, String& log );

	};

	inline CLProgram::CLProgram( cl_program p ) : CLObject<cl_program>( p )
	{
	}

	inline CLProgram::CLProgram( const CLProgram& p ) : CLObject<cl_program>( p._object )
	{
	}

	inline bool CLProgram::build( const std::vector<CLDevice>& devices, const char* options )
	{
		cl_int err;
		err = ::clBuildProgram( *this, devices.size(), ( cl_device_id* ) &devices[ 0 ], options, NULL, NULL );
		if( err == CL_BUILD_PROGRAM_FAILURE )
			return false;
		if( err != CL_SUCCESS )
			throw CLException( __PRETTY_FUNCTION__, err );
		return true;
	}

	inline bool CLProgram::build( const CLDevice& dev, const char* options )
	{
		cl_int err;
		err = ::clBuildProgram( *this, 1, ( cl_device_id* ) &dev, options, NULL, NULL );
		if( err == CL_BUILD_PROGRAM_FAILURE )
			return false;
		if( err != CL_SUCCESS )
			throw CLException( __PRETTY_FUNCTION__, err );
		return true;
	}

	inline void CLProgram::buildLog( const CLDevice& dev, String& log )
	{
		cl_int err;
		size_t size;
		err = ::clGetProgramBuildInfo( *this, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &size );
		if( err != CL_SUCCESS )
			throw CLException( __PRETTY_FUNCTION__, err );
		log.resize( size );
		err = ::clGetProgramBuildInfo( *this, dev, CL_PROGRAM_BUILD_LOG, size, &log[ 0 ], &size );
		if( err != CL_SUCCESS )
			throw CLException( __PRETTY_FUNCTION__, err );
	}

}

#endif
