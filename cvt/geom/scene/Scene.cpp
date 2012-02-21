/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/geom/scene/Scene.h>
#include <cvt/util/PluginManager.h>

namespace cvt {

	void Scene::load( const String& path, SceneLoader* loader )
	{
		if( !loader ) {
			loader = PluginManager::instance().getSceneLoaderForFilename( path );
			if( !loader ){
                String message( "No SceneLoader for file available: " );
                message += path;
				throw CVTException( message.c_str() );
            }
		}
		loader->load( *this, path );
	}
}
