#include <cvt/util/PluginManager.h>

namespace cvt {

	PluginManager& PluginManager::instance()
	{
		static PluginManager _instance;
		return _instance;
	}

}
