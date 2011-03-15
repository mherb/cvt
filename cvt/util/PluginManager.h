#ifndef CVT_PLUGINMANAGER_H
#define CVT_PLUGINMANAGER_H

#include <cvt/util/Plugin.h>
#include <cvt/util/PluginFile.h>
#include <cvt/gfx/IFilter.h>
#include <vector>

namespace cvt {
	class PluginManager {
		public:
			static PluginManager& instance();
			void registerPlugin( Plugin* plugin );
			void loadPlugin( const std::string& path );
			void loadDefault();

			const IFilter* getIFilter( size_t i ) const;
			size_t getIFilterSize() const;

		private:
			PluginManager();
			PluginManager( const PluginManager& );
			~PluginManager();

			std::vector<PluginFile*> _plugins;
			std::vector<IFilter*> _ifilters;
	};

	inline PluginManager::PluginManager()
	{
	}

	inline PluginManager::~PluginManager()
	{
		for( std::vector<IFilter*>::iterator it = _ifilters.begin(), end = _ifilters.end(); it != end; ++it  )
			delete *it;
		_ifilters.clear();

		for( std::vector<PluginFile*>::iterator it = _plugins.begin(), end = _plugins.end(); it != end; ++it  )
			delete *it;
		_plugins.clear();
	}

	inline void PluginManager::registerPlugin( Plugin* plugin )
	{
		if( !plugin )
			return;
		switch( plugin->pluginType() ) {
			case PLUGIN_IFILTER:
				_ifilters.push_back( ( IFilter* ) plugin );
				break;
			default:
				break;
		}
	}

	inline void PluginManager::loadPlugin( const std::string& path )
	{
		PluginFile* p = new PluginFile( path.c_str() );
		p->load();
		_plugins.push_back( p );
	}


	inline const IFilter* PluginManager::getIFilter( size_t n ) const
	{
		return _ifilters[ n ];
	}

	inline size_t PluginManager::getIFilterSize() const
	{
		return _ifilters.size();
	}

}

#endif
