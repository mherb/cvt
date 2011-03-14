#ifndef CVT_PLUGINFILE_H
#define CVT_PLUGINFILE_H

#include <cvt/util/Plugin.h>

namespace cvt {
	class PluginManager;

	class PluginFile {
		public:
			PluginFile( const char* path );
			~PluginFile();
			void load();
		private:
			void check();
			PluginFile( const PluginFile& );
			PluginFile& operator=( const PluginFile& );

			void* _handle;
			PluginInfo* _pinfo;
	};
}

#endif
