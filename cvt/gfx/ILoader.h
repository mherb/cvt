/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_ILOADER_H
#define CVT_ILOADER_H

#include <cvt/util/Plugin.h>
#include <cvt/util/String.h>
#include <cvt/gfx/Image.h>

namespace cvt {
	class ILoader : public Plugin
	{
		public:
			ILoader() : Plugin( PLUGIN_ILOADER ) {};

			virtual void load( Image& dst, const String& file ) = 0;
			virtual const String& extension( size_t n ) const = 0;
			virtual size_t sizeExtensions() const = 0;
			virtual const String& name() const = 0;
			bool isExtensionSupported( const String& suffix ) const;
	};

	inline bool ILoader::isExtensionSupported( const String& suffix ) const
	{
		for( size_t i = 0, end = sizeExtensions(); i < end; i++ ) {
			if( suffix == extension( i ) )
				return true;
		}
        return false;
	}
}

#endif
