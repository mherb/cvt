#ifndef CVT_PLUGINMANAGER_H
#define CVT_PLUGINMANAGER_H

#include <cvt/util/Plugin.h>

namespace cvt {
	class PluginManager {
		public:
			PluginManager& instance();
			void registerPlugin( Plugin* plugin );
			void loadPlugin( const std::string& path );

		private:
			PluginManager()
			PluginManager( const PluginManager& );
			~PluginManager();
	};

	inline PluginManager& PluginManager::instance()
	{
		static PluginManager instance;
		return instance;
	}

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

}

#endif
