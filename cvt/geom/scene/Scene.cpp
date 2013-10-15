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
