#include <cvt/cl/CLImageFormat.h>

namespace cvt {
		const char* CLImageFormat::orderToString( cl_channel_order order )
		{
			/* cl_channel_order */
			switch( order ) {
				case CL_R: return "R";
				case CL_A: return "A";
				case CL_RG: return "RG";
				case CL_RA: return "RA";
				case CL_RGB: return "RGB";
				case CL_RGBA: return "RGBA";
				case CL_BGRA: return "BGRA";
				case CL_ARGB: return "ARGB";
				case CL_INTENSITY: return "INTENSITY";
				case CL_LUMINANCE: return "LUMINANCE";
				default: return "UNKNOWN";
			}
		}

		const char* CLImageFormat::typeToString( cl_channel_type type  )
		{
			/* cl_channel_type */
			switch( type ) {
				case CL_SNORM_INT8: return "SNORM_INT8";
				case CL_SNORM_INT16: return "SNORM_INT16";
				case CL_UNORM_INT8: return "UNORM_INT8";
				case CL_UNORM_INT16: return "UNORM_INT16";
				case CL_UNORM_SHORT_565: return "UNORM_SHORT_565";
				case CL_UNORM_SHORT_555: return "UNORM_SHORT_555";
				case CL_UNORM_INT_101010: return "UNORM_INT_101010";
				case CL_SIGNED_INT8: return "SIGNED_INT8";
				case CL_SIGNED_INT16: return "SIGNED_INT16";
				case CL_SIGNED_INT32: return "SIGNED_INT32";
				case CL_UNSIGNED_INT8: return "UNSIGNED_INT8";
				case CL_UNSIGNED_INT16: return "UNSIGNED_INT16";
				case CL_UNSIGNED_INT32: return "UNSIGNED_INT32";
				case CL_HALF_FLOAT: return "HALF_FLOAT";
				case CL_FLOAT: return "FLOAT";
				default: return "UNKNOWN";
			}
		}
}
