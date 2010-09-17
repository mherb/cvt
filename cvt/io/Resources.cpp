#include "Resources.h"
#include "FileSystem.h"

#include <cvt/util/Exception.h>

#include <sstream>

namespace cvt {
	Resources::Resources()
	{
		searchFolders.push_back(std::string(DATA_FOLDER));
		searchFolders.push_back(".");		
	}
	
	Resources::~Resources()
	{		
	}
	
	std::string Resources::find( const std::string& resource )
	{
		std::stringstream ss;

		std::list<std::string>::iterator it = searchFolders.begin();
		std::list<std::string>::iterator end = searchFolders.end();
		while (it != end) {		
			ss << *it++ << "/" << resource;
			
			if( FileSystem::exists( ss.str() ) ){
				return ss.str();
			}
			
			ss.clear();
			ss.str("");
		}
		
		ss << "Resource not found: " << resource;
		throw CVTException(ss.str());
	}
	
	void Resources::addSearchLocation( const std::string& loc, bool prepend )
	{
		if( prepend )
			searchFolders.push_front( loc );
		else 
			searchFolders.push_back( loc );
	}
	
}