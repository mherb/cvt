#ifndef CVT_PLUGINMANAGER_H
#define CVT_PLUGINMANAGER_H

#include <cvt/util/Plugin.h>
#include <vector>

namespace cvt {
	class PluginManager {
		public:
			static PluginManager& instance();
			void registerPlugin( Plugin* plugin );
			void loadPlugin( const std::string& path );
			void loadDefault();

		private:
			PluginManager();
			PluginManager( const PluginManager& );
			~PluginManager();
	};

	inline PluginManager::PluginManager()
	{
	}

	inline PluginManager::~PluginManager()
	{
	}

	inline void PluginManager::registerPlugin( Plugin* plugin )
	{
	}

	inline void PluginManager::loadPlugin( const std::string& path )
	{
	}

	inline void PluginManager::loadDefault()
	{
	}
}

#endif
