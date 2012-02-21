/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_PLATFORM_H
#define CVT_PLATFORM_H

#include <vector>
#include <cvt/cl/CLUtil.h>
#include <cvt/cl/CLDevice.h>

namespace cvt {
	class CLPlatform
	{
		friend std::ostream& operator<<( std::ostream& out, const CLPlatform& clplat );
		public:
			CLPlatform( cl_platform_id id = NULL );
			~CLPlatform();

			static void get( std::vector<CLPlatform>& platforms );

			CLUTIL_GETINFOSTRING( name, CL_PLATFORM_NAME, _id, ::clGetPlatformInfo );
			CLUTIL_GETINFOSTRING( vendor, CL_PLATFORM_VENDOR, _id, ::clGetPlatformInfo );
			CLUTIL_GETINFOSTRING( profile, CL_PLATFORM_PROFILE, _id, ::clGetPlatformInfo );

			void devices( std::vector<CLDevice>& devices, cl_device_type type = CL_DEVICE_TYPE_ALL ) const;
			CLDevice defaultDevice() const;
			void extensions( std::vector<String>& extensions ) const;

			operator cl_platform_id () const { return _id; }
			operator cl_context_properties () const { return ( cl_context_properties )_id; }

		private:
			CLUTIL_GETINFOSTRING( _extensions, CL_PLATFORM_EXTENSIONS, _id, ::clGetPlatformInfo );

			cl_platform_id _id;
	};

	inline CLPlatform::CLPlatform( cl_platform_id id ) : _id( id )
	{
	}

	inline CLPlatform::~CLPlatform()
	{
	}

	inline void CLPlatform::get( std::vector<CLPlatform>& platforms )
	{
		cl_int err;
		cl_uint num;
		cl_platform_id* ids;

		err = ::clGetPlatformIDs( 0, NULL, &num );
		if( err != CL_SUCCESS )
			throw CLException( err );
		ids = ( cl_platform_id* ) alloca( sizeof( cl_platform_id ) * num );
		err = ::clGetPlatformIDs( num, ids, &num );
		if( err != CL_SUCCESS )
			throw CLException( err );
		platforms.assign( ids, ids + num );
	}

	inline void CLPlatform::devices( std::vector<CLDevice>& devices, cl_device_type type ) const
	{
		cl_int err;
		cl_uint num;
		cl_device_id* ids;

		err = ::clGetDeviceIDs( _id, type, 0, NULL, &num );
		if( err == CL_DEVICE_NOT_FOUND ) {
			devices.clear();
			return;
		}
		if( err != CL_SUCCESS )
			throw CLException( err );
		ids = ( cl_device_id* ) alloca( sizeof( cl_device_id ) * num );
		err = ::clGetDeviceIDs( _id, type, num, ids, &num );
		if( err != CL_SUCCESS )
			throw CLException( err );
		devices.assign( ids, ids + num );
	}

	inline CLDevice CLPlatform::defaultDevice() const
	{
		cl_int err;
		cl_device_id id;

		err = ::clGetDeviceIDs( _id, CL_DEVICE_TYPE_DEFAULT, 1, &id, NULL );
		if( err != CL_SUCCESS )
			throw CLException( err );
		return CLDevice( id );
	}

	inline void CLPlatform::extensions( std::vector<String>& extensions ) const
	{
		String ext;
		_extensions( ext );
		ext.tokenize( extensions, ' ' );
	}

	inline std::ostream& operator<<( std::ostream& out, const CLPlatform& clplat )
	{
		String str;
		clplat.vendor( str );
		out << "CLPlatform\n\tVendor: " << str;
		clplat.name( str );
		out << "\n\tName: " << str;
		clplat.profile( str );
		out << "\n\tProfile: " << str;
		clplat._extensions( str );
		out << "\n\tExtensions: " << str;
		return out;
	}

}

#endif
