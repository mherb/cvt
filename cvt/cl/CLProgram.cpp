/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/cl/CLProgram.h>
#include <cvt/cl/CLContext.h>

namespace cvt {

	CLProgram::CLProgram( const CLContext& context, const char* prog )
	{
		cl_int err;
		_object = ::clCreateProgramWithSource( context, 1, &prog, NULL, &err );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}

	CLProgram::CLProgram( const CLContext& context, const String& prog )
	{
		cl_int err;
		const char* str = prog.c_str();
		_object = ::clCreateProgramWithSource( context, 1, &str, NULL, &err );
		if( err != CL_SUCCESS )
			throw CLException( err );
	}

}
