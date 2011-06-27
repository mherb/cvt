#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <cvt/util/String.h>
#include <vector>
#include <cvt/util/Data.h>

namespace cvt {

	class FileSystem
	{
		public:
			static bool   exists( const String & path );
			static void   rename( const String & from, const String & to );
			static bool   isFile( const String & file );
			static bool   isDirectory( const String & dir );
			static void   ls( const String & path, std::vector<String> & entries );
			static void   filesWithExtension( const String & path, std::vector<String>& result, const String & ext = "" );
			static size_t size( const String& path );
			static bool	  load( Data& d, const String& path, bool zerotermination = false );
			static bool	  save( const String& path, const Data& d );
	};

}

#endif

