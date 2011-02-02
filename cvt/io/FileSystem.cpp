#include "FileSystem.h"
#include "util/Exception.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>

#include <cvt/util/CVTTest.h>

namespace cvt {

	bool FileSystem::exists( const std::string & path )
	{
		struct stat attr;
		if ( stat( path.c_str(), &attr ) == -1 )
			return false;
		else
			return true;
	}

    void FileSystem::rename(const std::string & from, const std::string & to)
    {
		if ( ::rename( from.c_str(), to.c_str() ) < 0 )
			throw CVTException("Could not rename file");
    }

    bool FileSystem::isFile(std::string const & file)
    {
		struct stat attr;
		if( stat( file.c_str(), &attr ) == -1 )
			CVTException( "Given path does not exist" );
		return S_ISREG( attr.st_mode );
    }

    bool FileSystem::isDirectory(std::string const & dir)
    {
		struct stat attr;
		if( stat( dir.c_str(), &attr ) == -1 )
			CVTException( "Given path does not exist" );
		return S_ISDIR( attr.st_mode );
    }

	void FileSystem::ls( const std::string & path, std::vector<std::string> & entries )
	{
		if( !exists( path ) ){
			throw CVTException( "Path not found: " + path );
		}

		DIR * dirEntries = opendir( path.c_str() );
		if( dirEntries == NULL ){
			CVTException( "Directory not readable: " + path );
		}

		struct dirent * entry = NULL;

		while( ( entry = readdir( dirEntries ) ) != NULL ) {
			std::string entryName( entry->d_name );

			if( entryName.compare( "." ) == 0 ||
			    entryName.compare( ".." ) == 0 )
				continue;

			entries.push_back( entryName );
		}
	}

    void FileSystem::filesWithExtension( std::string const& _path, std::vector<std::string>& result, std::string const & ext )
	{
		std::string path = _path;

		if( path[ path.length()-1 ] != '/' )
			path += '/';

		if( !exists( path ) ){
			throw CVTException( "Path not found: " + path );
		}

		DIR * dirEntries = opendir( path.c_str() );
		if( dirEntries == NULL ){
			CVTException( "Directory not readable: " + path );
		}

		struct dirent * entry = NULL;

		while( ( entry = readdir( dirEntries ) ) != NULL ) {
			std::string entryName( path + entry->d_name );

			if( isFile( entryName ) ) {
				if( entryName.length() >= ext.length() ){
					std::string entryExt = entryName.substr( entryName.length() - ext.length() );
					if( entryExt.compare( ext ) == 0 ){
						result.push_back( entryName );
					}
				}
			}
		}
	}



	BEGIN_CVTTEST( filesystem )
		bool result = true;
		std::string dataFolder( DATA_FOLDER );
		bool b = FileSystem::exists( "/usr/include" );
		b &= FileSystem::exists( dataFolder );
		b &= !FileSystem::exists( "bliblabluiamnothere" );
		CVTTEST_PRINT( "exists: ", b );
		result &= b;

		b = FileSystem::isDirectory( "/usr/include" );
		b &= !FileSystem::isDirectory( dataFolder + "/lena.png" );
		CVTTEST_PRINT( "isDirectory: ", b );
		result &= b;

		b = !FileSystem::isFile( "/usr/include" );
		b &= FileSystem::isFile( dataFolder + "/lena.png" );
		CVTTEST_PRINT( "isFile: ", b );
		result &= b;

		FileSystem::rename( dataFolder + "/lena.png", dataFolder + "/blubb.png" );
		b = FileSystem::exists( dataFolder + "/blubb.png" );
		b &= !FileSystem::exists( dataFolder + "/lena.png" );
		FileSystem::rename( dataFolder + "/blubb.png", dataFolder + "/lena.png" );
		b &= !FileSystem::exists( dataFolder + "/blubb.png" );
		b &= FileSystem::exists( dataFolder + "/lena.png" );
		CVTTEST_PRINT( "rename: ", b );
		result &= b;

		return result;
	END_CVTTEST
}
