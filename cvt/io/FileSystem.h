#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>

namespace cvt {

class FileSystem
{
    public:
		static bool exists( const std::string & path );
		static void rename( const std::string & from, const std::string & to );
		static bool isFile( std::string const & file );
		static bool isDirectory( std::string const & dir );
		static void ls( const std::string & path, std::vector<std::string> & entries );
		static void filesWithExtension( const std::string & path, std::vector<std::string>& result, const std::string & ext = "" );
};

}

#endif

