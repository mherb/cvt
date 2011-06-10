#ifndef CVT_PLUGINMANAGER_H
#define CVT_PLUGINMANAGER_H

#include <cvt/util/Plugin.h>
#include <cvt/util/PluginFile.h>
#include <cvt/gfx/IFilter.h>
#include <cvt/gfx/ILoader.h>
#include <cvt/gfx/ISaver.h>
#include <cvt/util/String.h>
#include <vector>
#include <map>

namespace cvt {
	class PluginManager {
		public:
			static PluginManager& instance();
			void registerPlugin( Plugin* plugin );
			void loadPlugin( const String& path );
			void loadDefault();

			IFilter* getIFilter( size_t i ) const;
			IFilter* getIFilter( const String& name ) const;
			size_t getIFilterSize() const;

		private:
			PluginManager();
			PluginManager( const PluginManager& );
			~PluginManager();

			std::vector<PluginFile*> _plugins;
			std::vector<IFilter*> _ifilters;
			std::vector<ILoader*> _iloaders;
			std::vector<ISaver*> _isavers;
			std::map< const String, IFilter*> _ifiltermap;
	};

	inline PluginManager::PluginManager()
	{
	}

	inline PluginManager::~PluginManager()
	{
		for( std::vector<IFilter*>::iterator it = _ifilters.begin(), end = _ifilters.end(); it != end; ++it  )
			delete *it;
		_ifilters.clear();

		for( std::vector<ILoader*>::iterator it = _iloaders.begin(), end = _iloaders.end(); it != end; ++it  )
			delete *it;
		_iloaders.clear();

		for( std::vector<ISaver*>::iterator it = _isavers.begin(), end = _isavers.end(); it != end; ++it  )
			delete *it;
		_isavers.clear();

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
			case PLUGIN_ILOADER:
				{
					std::cout << "Loaded ILoader " << plugin->name() << " Extension:" << ( ( ILoader* ) plugin )->extension( 0 ) << std::endl;
					_iloaders.push_back( ( ILoader* ) plugin );
				}
				break;
			case PLUGIN_ISAVER:
				{
					std::cout << "Loaded ISaver " << plugin->name() << " Extension:" << ( ( ISaver* ) plugin )->extension( 0 ) << std::endl;
					_isavers.push_back( ( ISaver* ) plugin );
				}
				break;
			default:
				break;
		}
	}

	inline void PluginManager::loadPlugin( const String& path )
	{
		PluginFile* p = new PluginFile( path.c_str() );
		p->load();
		_plugins.push_back( p );
	}


	inline IFilter* PluginManager::getIFilter( size_t n ) const
	{
		return _ifilters[ n ];
	}

	inline IFilter* PluginManager::getIFilter( const String& name ) const
	{
		std::map< const String, IFilter*>::const_iterator it;
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
