#ifndef CVT_PLUGINMANAGER_H
#define CVT_PLUGINMANAGER_H

#include <cvt/util/Plugin.h>
#include <cvt/util/PluginFile.h>
#include <cvt/gfx/IFilter.h>
#include <vector>
#include <map>

namespace cvt {
	class PluginManager {
		public:
			static PluginManager& instance();
			void registerPlugin( Plugin* plugin );
			void loadPlugin( const std::string& path );
			void loadDefault();

			const IFilter* getIFilter( size_t i ) const;
			const IFilter* getIFilter( const std::string& name ) const;
			size_t getIFilterSize() const;

		private:
			PluginManager();
			PluginManager( const PluginManager& );
			~PluginManager();

			std::vector<PluginFile*> _plugins;
			std::vector<IFilter*> _ifilters;
			std::map< const std::string, IFilter*> _ifiltermap;
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
		_ifiltermap.clear();
	}

	inline void PluginManager::registerPlugin( Plugin* plugin )
	{
		if( !plugin )
			return;
		switch( plugin->pluginType() ) {
			case PLUGIN_IFILTER:
				{
					_ifilters.push_back( ( IFilter* ) plugin );
					_ifiltermap[ plugin->name() ] = ( IFilter* ) plugin;
				}
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

	inline const IFilter* PluginManager::getIFilter( const std::string& name ) const
	{
		std::map< const std::string, IFilter*>::const_iterator it;
		if( ( it = _ifiltermap.find( name ) ) != _ifiltermap.end() ) {
			return it->second;
		}
		return NULL;
	}

	inline size_t PluginManager::getIFilterSize() const
	{
		return _ifilters.size();
	}

}

#endif
