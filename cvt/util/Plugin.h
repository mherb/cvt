#ifndef CVT_PLUGIN_H
#define CVT_PLUGIN_H

#include <stdint.h>
#include <string>

namespace cvt {
	class PluginManager;

	struct PluginInfo {
		uint32_t magic;
		uint32_t major;
		uint32_t minor;
		void (*init)( PluginManager* manager );
	};

	enum PluginType {
		PLUGIN_IMAGE_LOADER,
		PLUGIN_IMAGE_SAVER,
		PLUGIN_IFILTER,
	};

	class Plugin {
		public:
			Plugin( PluginType type );
			virtual ~Plugin();
			virtual const std::string& name() const = 0;
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
