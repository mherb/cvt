#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>

namespace cvt {

class FileSystem
{
    public:
	static bool exists(std::string const & path);
	static void rename(const std::string & from, const std::string & to);
	static bool isFile(std::string const & file);
	static bool isDirectory(std::string const & dir);
	static void getFilesWithExtension(std::string const &path, std::vector<std::string>& result, std::string const & ext = "");
};

}

#endif

