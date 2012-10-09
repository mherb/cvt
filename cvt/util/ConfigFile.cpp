/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#include <cvt/io/FileSystem.h>
#include <cvt/util/ConfigFile.h>
#include <cvt/util/Data.h>
#include <cvt/util/DataIterator.h>
#include <fstream>

namespace cvt {

    ConfigFile::ConfigFile( const String& filename )
    {
        if( FileSystem::exists( filename ) ){
            Data data;
            FileSystem::load( data, filename );
            DataIterator it( data );

            String currentGroup = "default";

            while( it.hasNext() ){
                std::vector<String> tokens;
                it.tokenizeNextLine( tokens, "= " );
                if( tokens.size() == 2 ){                    
                    _groups[ currentGroup ][ tokens[ 0 ] ] = tokens[ 1 ];
                }
            }
        }
    }

    void ConfigFile::save( const String& filename ) const
    {
        std::ofstream file;
        file.open( filename.c_str() );

        GroupMapType::const_iterator groupIter = _groups.begin();
        GroupMapType::const_iterator groupEnd = _groups.end();

        while( groupIter != groupEnd ){
            file << "[" << groupIter->first << "]" << std::endl;
            writeGroup( file, groupIter->second );
            groupIter++;

            file << std::endl;
        }

        file.close();
    }    

    void ConfigFile::writeGroup( std::ofstream& out, const MapType& map ) const
    {
        MapType::const_iterator it = map.begin();
        const MapType::const_iterator end = map.end();

        while( it != end ){
            out << it->first << " = " << it->second << std::endl;
            it++;
        }
    }

    template <>
    String ConfigFile::valueFromString<String>( const String& strVal ) const
    {
        return strVal;
    }

    template <>
    String  ConfigFile::stringFromValue<String>( const String& val ) const
    {
        return val;
    }

    template <>
    int ConfigFile::valueFromString<int>( const String& strVal ) const
    {
        return strVal.toInteger();
    }

    template <>
    String  ConfigFile::stringFromValue<int>( const int& val ) const
    {
        String sval;
        sval.sprintf( "%d", val );
        return sval;
    }

    template <>
    float ConfigFile::valueFromString<float>( const String& strVal ) const
    {
        return strVal.toFloat();
    }

    template <>
    String  ConfigFile::stringFromValue<float>( const float& val ) const
    {
        String sval;
        sval.sprintf( "%0.6f", val );
        return sval;
    }

    template <>
    double ConfigFile::valueFromString<double>( const String& strVal ) const
    {
        return strVal.toFloat();
    }

    template <>
    String  ConfigFile::stringFromValue<double>( const double& val ) const
    {
        String sval;
        sval.sprintf( "%0.10g", val );
        return sval;
    }

    template <>
    bool ConfigFile::valueFromString<bool>( const String& strVal ) const
    {
        String lower( strVal );
        lower.toLower();
        if( lower == "true" ||
            strVal == "1" ){
            return true;
        } else {
            return false;
        }
    }

    template <>
    String  ConfigFile::stringFromValue<bool>( const bool& val ) const
    {
        if( val )
            return "true";
        return "false";
    }
}
