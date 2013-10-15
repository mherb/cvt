#ifndef CVT_RESOURCES_H
#define CVT_RESOURCES_H

#include <cvt/util/String.h>
#include <list>

namespace cvt {
	/**
	  \class cvt::Resources io/Resources.h
	  \brief Easy search for paths of resource files.
			 The default search order is: DataFolder, current directory.
	 */
	class Resources
	{
		public:
			Resources();
			~Resources();

			/**
			  Find absolute path to a resource.
			  \param resource the name of the resource.
			  \return the absolute path to the resource.
			 */
			String find( const String & resource );

			/**
			  Add another location to search for resources.
				\param loc location to be added
				\param prepend put location in front of all search-locations.
			 */
			void addSearchLocation( const String& loc, bool prepend=true );

		private:
			std::list<String> searchFolders;
	};
}
#endif
