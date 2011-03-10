#ifndef CVT_PLUGIN_H
#define CVT_PLUGIN_H

namespace cvt {
	class PluginManager;

	struct PluginInfo {
		uint32_t magic;
		uint32_t minor;
		uint32_t major;
		void (*init)( PluginManager* manager );
	};

	enum PluginType {
		PLUGIN_IMAGE_LOADER,
		PLUGIN_IMAGE_SAVER,
		PLUGIN_IFILTER
	};

	class Plugin {
		public:
			Plugin( const std::string& name, PluginType type );
			~Plugin();
			const std::string& pluginName() const;
			PluginType pluginType() const;

		private:
			std::string _name;
			PluginType  _type;
	};

	inline Plugin::Plugin( const std::string& name, PluginType type ) : _name( name ), _type( type )
	{
	}

	inline Plugin::~Plugin()
	{
	}

	inline std::string& Plugin::pluginName() const
	{
		return _name;
	}

	inline PluginType Plugin::pluginType() const
	{
		return _type;
	}

}

#endif
