#include <cvt/util/PluginManager.h>
#include <cvt/util/Exception.h>
#include <cvt/io/FileSystem.h>
#include <cvt/util/String.h>
#include <string>
#include <vector>

namespace cvt {

	PluginManager& PluginManager::instance()
	{
		static PluginManager _instance;
		return _instance;
	}

	void PluginManager::loadDefault()
	{
		std::vector<std::string> entries;
		FileSystem::ls( PLUGIN_PATH, entries );
		for( std::vector<std::string>::iterator it = entries.begin(), end = entries.end(); it != end; ++it ) {
			try {
				String path( PLUGIN_PATH );
				path += it->c_str();
				loadPlugin( path.c_str() );
			} catch( Exception e ) {
			}
		}
	}
}
