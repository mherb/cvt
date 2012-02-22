/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_SCENELOADER_H
#define CVT_SCENELOADER_H

#include <cvt/util/Plugin.h>
#include <cvt/util/String.h>
#include <cvt/geom/scene/Scene.h>

namespace cvt {
	class SceneLoader : public Plugin
	{
		public:
			SceneLoader() : Plugin( PLUGIN_SCENELOADER ) {};

			virtual void load( Scene& dst, const String& file ) = 0;
			virtual const String& extension( size_t n ) const = 0;
			virtual size_t sizeExtensions() const = 0;
			virtual const String& name() const = 0;
			bool isExtensionSupported( const String& suffix ) const;
	};

	inline bool SceneLoader::isExtensionSupported( const String& suffix ) const
	{
		for( size_t i = 0, end = sizeExtensions(); i < end; i++ ) {
			if( suffix == extension( i ) )
				return true;
		}
        return false;
	}
}

#endif
