
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
            T   valueForName( const String& name, T defaultValue, const String& group = "" );

            void save( const String& filename ) const;

        private:           
            typedef std::map<String, String> MapType;            
            typedef std::map<String, MapType> GroupMapType;

            GroupMapType _groups;

            template <class T>
            T   valueFromString( const String& strVal ) const;

            template <class T>
            String  stringFromValue( const T& val ) const;

            void writeGroup( std::ofstream& out, const MapType& map ) const;
    };

    template <class T>
    inline T ConfigFile::valueForName( const String& name, T defaultValue, const String& group )
    {
        String queryGrp = group;
        if( queryGrp == "" ){
            queryGrp = "default";
        }

        GroupMapType::iterator groupIter = _groups.find( queryGrp );
        if( groupIter == _groups.end() ){
            // group not present -> create
            groupIter = _groups.insert( std::make_pair( queryGrp, MapType() ) ).first;
        }

        // groupIter is the map with the values:
        MapType::const_iterator valIter = groupIter->second.find( name );
        if( valIter == groupIter->second.end() ){
            // not present: add default value:
            groupIter->second.insert( std::make_pair( name, stringFromValue( defaultValue ) ) );
            return defaultValue;
        } else {
            return valueFromString<T>( valIter->second );
        }
    }
}

#endif // CONFIGFILE_H
