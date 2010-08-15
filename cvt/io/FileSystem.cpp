#include "FileSystem.h"
#include "util/CVTException.h"
#include <boost/filesystem.hpp>

namespace cvt {

    bool FileSystem::exists(std::string const & path)
    {
	if(boost::filesystem::is_directory(path))
	    return true;
	if(boost::filesystem::exists(path))
	    return true;
	return false;
    }

    void FileSystem::rename(const std::string & from, const std::string & to)
    {
	boost::filesystem::rename(from, to);
    }

    bool FileSystem::isFile(std::string const & file)
    {
	return boost::filesystem::is_regular_file(file);
    }

    bool FileSystem::isDirectory(std::string const & dir)
    {
	return boost::filesystem::is_directory(dir);
    }

    void FileSystem::getFilesWithExtension(std::string const &path, std::vector<std::string>& result, std::string const & ext)
    {
	boost::filesystem::path full_path(boost::filesystem::initial_path());
	full_path = boost::filesystem::system_complete(boost::filesystem::path(path, boost::filesystem::native));

	if (!boost::filesystem::exists(full_path)){
	    throw CVTException("\nNot found: " + full_path.native_file_string());
	}

	if (boost::filesystem::is_directory(full_path)){
	    boost::filesystem::directory_iterator end_iter;

	    for (boost::filesystem::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr) {
		try {
		    // is this a file?
		    if (boost::filesystem::is_regular(*dir_itr)) {
			std::string name = dir_itr->filename();

			if (name.size() >= ext.size()) {
			    // does it have the right extension (postfix)?
			    if (name.substr(name.size() - ext.size(), ext.size()) == ext) {
				result.push_back(name);
			    }
			}
		    }
		} catch (const std::exception & ex) {
		    CVTException(dir_itr->filename() + " " + ex.what());
		}
	    }
	} else {
	    throw CVTException("\n'" + path + "' is a file, but must be a directory.");
	}
    }

}
