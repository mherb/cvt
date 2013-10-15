#include <cvt/util/PluginFile.h>
#include <cvt/util/PluginManager.h>
#include <cvt/util/Exception.h>

#include <dlfcn.h>


namespace cvt {

	PluginFile::PluginFile( const char* path ) : _handle( NULL )
	{
		if( !( _handle = dlopen( path, RTLD_LAZY | RTLD_LOCAL ) ) ) {
			std::string msg( "Unable to open plugin \"" );
			msg += path;
			msg += "\" :";
			msg += dlerror();
			throw CVTException( msg );
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
		if( _pinfo->magic != CVT_PLUGIN_MAGIC ||
		    _pinfo->major != CVT_PLUGIN_MAJOR ||
		    _pinfo->minor != CVT_PLUGIN_MINOR ||
			!_pinfo->init) {
			dlclose( _handle );
			_handle = NULL;
			_pinfo = NULL;
			throw CVTException("Incompatible plugin !");
		}
	}

	void PluginFile::load()
	{
		if( !_pinfo || !_handle )
			return;
		_pinfo->init( &PluginManager::instance() );
	}
}
