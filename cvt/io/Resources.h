#ifndef CVT_RESOURCES_H
#define CVT_RESOURCES_H

#include <string>
#include <list>

namespace cvt {
	
	class Resources
	{
		public:
			/**
			 * @class cvt/io/Resources.h
			 * @brief easy search for paths of resource files
			 * @description the default search order is: DataFolder, Current Directory
			 */
			Resources();
			~Resources();
		
			std::string find( const std::string& resource );
		
			/**
			 * adds another location to search for resources. 
			 */
			void addSearchLocation( const std::string& loc, bool prepend=true );
		
		private:
			std::list<std::string> searchFolders;		
	};	
}
#endif