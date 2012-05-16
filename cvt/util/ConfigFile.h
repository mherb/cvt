/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
*/

#ifndef CVT_CONFIGFILE_H
#define CVT_CONFIGFILE_H

#include <cvt/util/String.h>
#include <map>

namespace cvt {

    class ConfigFile
    {
        public:
            ConfigFile( const String& filename );

            template <class T>
            T   valueForName( const String& name, T defaultValue );

            void save( const String& filename ) const;

        private:
            typedef std::map<String, String> MapType;
            MapType     _values;

    };
}

#endif // CONFIGFILE_H
