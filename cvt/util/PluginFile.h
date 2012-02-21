/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
