#include <cvt/util/PluginManager.h>
#include <cvt/util/Exception.h>
#include <cvt/io/FileSystem.h>
#include <cvt/util/String.h>
#include <string>
#include <vector>

namespace cvt {

	PluginManager& PluginManager::instance()
	{
		static PluginManager* _instance = NULL;
		if( !_instance ) {
			_instance = new PluginManager();
			_instance->loadDefault();
		}
		return *_instance;
	}

	void PluginManager::loadDefault()
	{
		// Static default plugins

		// Runtime loaded plugins
		std::vector<std::string> entries;
		FileSystem::ls( PLUGIN_PATH, entries );
		for( std::vector<std::string>::iterator it = entries.begin(), end = entries.end(); it != end; ++it ) {
			try {
				String path( PLUGIN_PATH );
				path += it->c_str();
				//std::cout << path << std::endl;
				loadPlugin( path.c_str() );
			} catch( Exception e ) {
			}
		}
	}
}
