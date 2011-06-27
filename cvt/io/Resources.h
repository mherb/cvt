#ifndef CVT_RESOURCES_H
#define CVT_RESOURCES_H

#include <cvt/util/String.h>
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

			String find( const String & resource );

			/**
			 * add another location to search for resources.
			 */
			void addSearchLocation( const String& loc, bool prepend=true );

		private:
			std::list<String> searchFolders;
	};
}
#endif
