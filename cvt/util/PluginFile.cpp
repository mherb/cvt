#include <cvt/util/PluginFile.h>
#include <cvt/util/PluginManager.h>
#include <cvt/util/Exception.h>

#include <dlfcn.h>


namespace cvt {

	PluginFile::PluginFile( const char* path ) : _handle( NULL )
	{
		if( !( _handle = dlopen( path, RTLD_LAZY | RTLD_LOCAL ) ) ) {
			throw CVTException( "Unable to load plugin!" );
		}
		check();
	}

	PluginFile::~PluginFile()
	{
		if( _handle )
			dlclose( _handle );
	}

	void PluginFile::check()
	{
		if( !( _pinfo = ( PluginInfo* ) dlsym( _handle, "_cvtplugin" ) ) ) {
			dlclose( _handle );
			_handle = NULL;
			throw CVTException("Unable to load plugin !");
		}
		if( _pinfo->magic != 0x43565450 ||
		    _pinfo->major != 0 ||
		    _pinfo->minor != 1 ) {
			dlclose( _handle );
			_handle = NULL;
			_pinfo = NULL;
			throw CVTException("Incompatible plugin !");
		}
	}

	void PluginFile::init( PluginManager* manager )
	{
		if( !_pinfo || !_handle )
			return;
		_pinfo->init( manager );
	}
}
