#include <cvt/cl/CLDevice.h>
#ifndef CVT_PLATFORM_H
#define CVT_PLATFORM_H

#include <vector>
#include <cvt/cl/CLUtil.h>
#include <cvt/cl/CLDevice.h>

namespace cvt {
	class CLPlatform
	{
		public:
			CLPlatform( cl_platform_id id = NULL );
			~CLPlatform();

			static void get( std::vector<CLPlatform>& platforms );

			CLUTIL_GETINFOSTRING( name, CL_PLATFORM_NAME, _id, ::clGetPlatformInfo );
			CLUTIL_GETINFOSTRING( vendor, CL_PLATFORM_VENDOR, _id, ::clGetPlatformInfo );
			CLUTIL_GETINFOSTRING( profile, CL_PLATFORM_PROFILE, _id, ::clGetPlatformInfo );
			CLUTIL_GETINFOSTRING( extensions, CL_PLATFORM_EXTENSIONS, _id, ::clGetPlatformInfo );

			void devices( std::vector<CLDevice>& devices, cl_device_type type = CL_DEVICE_TYPE_ALL ) const;

			operator cl_platform_id () const { return _id; }
			operator cl_context_properties () const { return ( cl_context_properties )_id; }

		private:
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
		if( err != CL_SUCCESS )
			throw CLException( err );
		ids = ( cl_device_id* ) alloca( sizeof( cl_device_id ) * num );
		err = ::clGetDeviceIDs( _id, type, num, ids, &num );
		if( err != CL_SUCCESS )
			throw CLException( err );
		devices.assign( ids, ids + num );
	}

	inline std::ostream& operator<<( std::ostream& out, const CLPlatform& clplat )
	{
		std::string str;
		clplat.vendor( str );
		out << "CLPlatform\n\tVendor: " << str;
		clplat.name( str );
		out << "\n\tName: " << str;
		clplat.profile( str );
		out << "\n\tProfile: " << str;
		clplat.extensions( str );
		out << "\n\tExtensions: " << str;
		return out;
	}

}

#endif
#include <cvt/cl/CLPlatform.h>

namespace cvt {

	CLPlatform CLDevice::platform() const
	{
		return CLPlatform( devicePlatform() );
	}
}
