/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_CLUTIL_H
#define CVT_CLUTIL_H

#include <alloca.h>
#include <cvt/cl/CLException.h>
#include <cvt/util/String.h>

#define CLUTIL_GETINFOSTRING( name, info, clobj, clobjinfofunc ) \
	void name( String& string ) const\
{	\
	cl_int err; \
	size_t size; \
	err = clobjinfofunc( clobj, info, 0, NULL, &size ); \
	if( err != CL_SUCCESS ) \
		throw CLException( err ); \
	string.resize( size ); \
	err = clobjinfofunc( clobj, info, size, &string[ 0 ], 0 ); \
	if( err != CL_SUCCESS ) \
		throw CLException( err ); \
}

#define CLUTIL_GETINFOTYPE( name, info, type, clobj, clobjinfofunc ) \
	type name() const\
{	\
	cl_int err; \
	type ret; \
	err = clobjinfofunc( clobj, info, sizeof( type ), &ret, NULL ); \
	if( err != CL_SUCCESS ) \
		throw CLException( err ); \
	return ret; \
}

#endif
