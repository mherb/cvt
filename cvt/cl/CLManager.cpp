#include <cvt/cl/CLManager.h>
#include <cvt/io/FileSystem.h>
#include <dlfcn.h>

namespace cvt {
	namespace cl {

#if defined( __APPLE__ ) || defined( __MACOSX )
		const char* CLManager::PLUGIN_SUFFIX = ".dylib";
#else
		const char* CLManager::PLUGIN_SUFFIX = ".so";
#endif

		const char* CLManager::getImageFormatChannelOrderString( ::cl::ImageFormat format )
		{
			/* cl_channel_order */
			switch( format.image_channel_order ) {
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

		const char* CLManager::getImageFormatChannelTypeString( ::cl::ImageFormat format )
		{
			/* cl_channel_type */
			switch( format.image_channel_data_type ) {
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

		CLManager::CLManager( ) : cunits( 0 ), maxworkgroupsize( 0 )
		{
			cl_int err;
			std::vector< ::cl::Platform> platforms;
			std::vector< ::cl::Device> devices;

			err = ::cl::Platform::get( &platforms );
			if( err != CL_SUCCESS )
				throw CLException( __PRETTY_FUNCTION__, err );

			platform = ::cl::Platform( platforms.front( ) );
			//platform.getDevices( CL_DEVICE_TYPE_CPU, &devices );
			err = platform.getDevices( CL_DEVICE_TYPE_DEFAULT, &devices );
			if( err != CL_SUCCESS )
				throw CLException( __PRETTY_FUNCTION__, err );
			device = ::cl::Device( devices.front( ) );

			context = ::cl::Context( devices, NULL, NULL, NULL, &err );
			if( err != CL_SUCCESS )
				throw CLException( __PRETTY_FUNCTION__, err );
			queue = ::cl::CommandQueue( context, device, NULL, &err );
			if( err != CL_SUCCESS )
				throw CLException( __PRETTY_FUNCTION__, err );

			device.getInfo( CL_DEVICE_MAX_COMPUTE_UNITS, &cunits );
			std::cout << "COMPUTING UNITS: " << cunits << std::endl;

			device.getInfo( CL_DEVICE_MAX_WORK_GROUP_SIZE, &maxworkgroupsize );
			std::cout << "MAX WORK-GROUP SIZE: " << maxworkgroupsize << std::endl;

			device.getInfo( CL_DEVICE_MAX_WORK_ITEM_SIZES, &maxworkdim );

			loadFilters( );
		}

		void CLManager::loadFilters( void )
		{
			CLFilter* filter;
			std::vector<std::string> files;

			FileSystem::getFilesWithExtension( PLUGIN_PATH, files, PLUGIN_SUFFIX );
			for( std::vector<std::string>::iterator iter = files.begin( ); iter != files.end( ); iter++ ) {
				std::string path = PLUGIN_PATH + *iter;

				filter = loadFilter( path.c_str( ) );
				if( filter ) {
					if( filter->isExecuteable( ) ) {
						std::cout << "Loaded filter " << filter->getName( ) << std::endl;
						//	       std::cout << *filter;
						filters[ filter->getName( ) ] = filter;
					} else {
						std::cout << "Filter " << filter->getName( ) << " has errors..." << std::endl;
					}
				}
			}
		}

		CLFilter* CLManager::loadFilter( const char* path )
		{
			CLFilterPlugin* plugin;
			CLFilter* ret;
			void* handle;

			handle = dlopen( path, RTLD_NOW | RTLD_LOCAL | RTLD_NODELETE );
			if( !handle ) {
				std::cout << "No handle:" << dlerror( ) << std::endl;
				return 0;
			}

			plugin = reinterpret_cast<CLFilterPlugin*>( dlsym( handle, "_clfilter" ) );
			if( !plugin )
				goto error;
			if( plugin->ID != 0xC7C7C7C7 )
				goto error;
			ret = plugin->getFilter( this );

			/* MacOS X dose not know RTLD_NODELETE */
			//dlclose( handle );
			return ret;

error:
			dlclose( handle );
			return 0;
		}

		CLManager::~CLManager( )
		{
			for( std::map<const std::string, CLFilter*>::iterator iter = filters.begin( ); iter != filters.end( ); ++iter )
				delete iter->second;
			filters.clear( );
			::cl::UnloadCompiler( );
		}

		void CLManager::infoImageFormats( std::ostream& out )
		{
			cl_int err;
			std::vector< ::cl::ImageFormat> imgformats;

			err = context.getSupportedImageFormats( CL_MEM_READ_WRITE, CL_MEM_OBJECT_IMAGE2D, &imgformats );
			if( err != CL_SUCCESS )
				throw CLException( __PRETTY_FUNCTION__, err );
			out << imgformats.size( ) << " image formats:" << std::endl;
			for( std::vector< ::cl::ImageFormat>::iterator it = imgformats.begin( ); it != imgformats.end( ); it++ ) {
				out << "\tChannel(s): \"" << getImageFormatChannelOrderString( *it ) << "\" Type: " << getImageFormatChannelTypeString( *it ) << std::endl;
			}
		}

		void CLManager::info( std::ostream& out )
		{
			std::vector< ::cl::Platform> platforms;
			size_t nump = 0;
			size_t numd;

			::cl::Platform::get( &platforms );

			for( std::vector< ::cl::Platform>::iterator iter = platforms.begin( ); iter != platforms.end( ); ++iter ) {
				std::string info;
				std::vector< ::cl::Device> devices;

				out << "Platform " << ++nump << ":" << std::endl;
				iter->getInfo( CL_PLATFORM_NAME, &info );
				out << "\tName: " << info << std::endl;
				iter->getInfo( CL_PLATFORM_VERSION, &info );
				out << "\tVersion: " << info << std::endl;
				iter->getInfo( CL_PLATFORM_VENDOR, &info );
				out << "\tVendor: " <<  info << std::endl;

				numd = 0;
				iter->getDevices( CL_DEVICE_TYPE_ALL, &devices );
				for( std::vector< ::cl::Device>::iterator iterd = devices.begin( ); iterd != devices.end( ); ++iterd ) {
					cl_device_type type;
					out << "\tDevice " << ++numd << ":" << std::endl;
					iterd->getInfo( CL_DEVICE_TYPE, &type );
					if( type == CL_DEVICE_TYPE_CPU )
						out << "\t\tType: CPU" << std::endl;
					else if( type == CL_DEVICE_TYPE_GPU )
						out << "\t\tType: GPU" << std::endl;
					else
						out << "\t\tType: OTHER" << std::endl;
					iterd->getInfo( CL_DEVICE_EXTENSIONS, &info );
					out << "\t\tExtensions: " << info << std::endl;
				}
			}
		}

	}
}
