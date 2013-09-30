/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include "Resources.h"
#include "FileSystem.h"

#include <cvt/util/Exception.h>

#include <sstream>

namespace cvt {
	Resources::Resources()
	{
		searchFolders.push_back( CVT_DATA_FOLDER );
		searchFolders.push_back( "." );
	}

	Resources::~Resources()
	{
	}

	String Resources::find( const String& resource )
	{
		std::list<String>::iterator it  = searchFolders.begin();
		std::list<String>::iterator end = searchFolders.end();

        String rappend( "/" );
        rappend += resource;

		while (it != end) {
            String s( *it++ );
            s += rappend;

			if( FileSystem::exists( s ) ){
				return s;
			}
		}

		String s( "Resource not found: " );
        s += resource;
		throw CVTException( s.c_str() );
	}

	void Resources::addSearchLocation( const String& loc, bool prepend )
	{
		if( prepend )
			searchFolders.push_front( loc );
		else
			searchFolders.push_back( loc );
	}

}
