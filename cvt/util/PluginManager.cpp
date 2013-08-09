/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/util/PluginManager.h>
#include <cvt/util/Exception.h>
#include <cvt/io/FileSystem.h>
#include <cvt/util/String.h>
#include <cvt/util/Util.h>
#include <vector>

namespace cvt {
	PluginManager* PluginManager::_instance = NULL;

	PluginManager& PluginManager::instance()
	{
		if( !_instance ) {
			_instance = new PluginManager();
			_instance->loadDefault();
		}
		return *_instance;
	}

	PluginManager::PluginManager()
	{
		// add folders to plugin path vector:
		if( FileSystem::exists( PLUGIN_PATH ) ){
			_pluginPaths.push_back( PLUGIN_PATH );
		}

		if( FileSystem::exists( "/usr/share/cvt/plugins" ) ){
			_pluginPaths.push_back( "/usr/share/cvt/plugins/" );
		}

		String envVar;
		if( Util::getEnv( envVar, "CVT_PLUGIN_PATH" ) ){
			_pluginPaths.push_back( envVar );
		}
	}

	void PluginManager::loadDefault()
	{
		// Static default plugins

		// Runtime loaded plugins
		std::vector<String> entries;
		for( size_t f = 0; f < _pluginPaths.size(); f++ ){
			FileSystem::ls( _pluginPaths[ f ], entries );
			for( std::vector<String>::iterator it = entries.begin(), end = entries.end(); it != end; ++it ) {
				String path( _pluginPaths[ f ] );
				try {
					path += *it;
					//std::cout << path << std::endl;
					loadPlugin( path.c_str() );
				} catch( Exception e ) {
					std::cout << "Could not load plugin at path: " << path << std::endl;
				}
			}
		}
	}

	void PluginManager::cleanup()
	{
		if( _instance )
			delete _instance;
	}

}
