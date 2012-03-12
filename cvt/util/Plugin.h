/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_PLUGIN_H
#define CVT_PLUGIN_H

#include <stdint.h>
#include <cvt/util/String.h>
#include <cvt/util/Version.h>

namespace cvt {
	class PluginManager;

	struct PluginInfo {
		uint32_t magic;
		uint32_t major;
		uint32_t minor;
		void (*init)( PluginManager* manager );
	};

#define CVT_PLUGIN_MAGIC 0x43565450
#define CVT_PLUGIN_MAJOR CVT_VERSION_MAJOR
#define CVT_PLUGIN_MINOR CVT_VERSION_MINOR

#define CVT_PLUGIN( initfunc ) \
	extern "C" { \
		cvt::PluginInfo _cvtplugin = { CVT_PLUGIN_MAGIC, CVT_PLUGIN_MAJOR , CVT_PLUGIN_MINOR, ( void ( * )( cvt::PluginManager* ) ) initfunc }; \
	}

	enum PluginType {
		PLUGIN_ILOADER,
		PLUGIN_ISAVER,
		PLUGIN_IFILTER,
		PLUGIN_SCENELOADER,
		PLUGIN_SCENESAVER
	};

	class Plugin {
		public:
			Plugin( PluginType type );
			virtual ~Plugin();
			virtual const String& name() const = 0;
			PluginType pluginType() const;

		private:
			PluginType  _type;
	};

	inline Plugin::Plugin( PluginType type ) : _type( type )
	{
	}

	inline Plugin::~Plugin()
	{
	}

	inline PluginType Plugin::pluginType() const
	{
		return _type;
	}

}

#endif
