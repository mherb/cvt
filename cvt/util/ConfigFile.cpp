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

            while( it.hasNext() ){
                std::vector<String> tokens;
                it.tokenizeNextLine( tokens, "= " );
                if( tokens.size() == 2 ){
                    _values[ tokens[ 0 ] ] = tokens[ 1 ];
                }
            }
        }
    }

    void ConfigFile::save( const String& filename ) const
    {
        std::ofstream file;
        file.open( filename.c_str() );

        MapType::const_iterator it = _values.begin();
        const MapType::const_iterator end = _values.end();

        while( it != end ){
            file << it->first << " = " << it->second << std::endl;
            it++;
        }

        file.close();
    }

    template <>
    String ConfigFile::valueForName<String>( const String& name, String defaultValue )
    {
        MapType::const_iterator it = _values.find( name );
        if( it == _values.end() ){
            _values[ name ] = defaultValue;
            return defaultValue;
        } else {
            return it->second;
        }
    }

    template <>
    int ConfigFile::valueForName<int>( const String& name, int defaultValue )
    {
        MapType::const_iterator it = _values.find( name );
        if( it == _values.end() ){
            String sval;
            sval.sprintf( "%d", defaultValue );
            _values[ name ] = sval;
            return defaultValue;
        } else {
            return it->second.toInteger();
        }
    }

    template <>
    float ConfigFile::valueForName<float>( const String& name, float defaultValue )
    {
        MapType::const_iterator it = _values.find( name );
        if( it == _values.end() ){
            String sval;
            sval.sprintf( "%0.6f", defaultValue );
            _values[ name ] = sval;
            return defaultValue;
        } else {
            return it->second.toFloat();
        }
    }

    template <>
    double ConfigFile::valueForName<double>( const String& name, double defaultValue )
    {
        MapType::const_iterator it = _values.find( name );
        if( it == _values.end() ){
            String sval;
            sval.sprintf( "%0.10g", defaultValue );
            _values[ name ] = sval;
            return defaultValue;
        } else {
            return it->second.toDouble();
        }
    }

    template <>
    bool ConfigFile::valueForName<bool>( const String& name, bool defaultValue )
    {
        MapType::const_iterator it = _values.find( name );
        if( it == _values.end() ){
            if( defaultValue ){
                _values[ name ] = "true";
            } else {
                _values[ name ] = "false";
            }
            return defaultValue;
        } else {
            String lower( it->second );
            lower.toLower();
            if( lower == "true" ||
                it->second == "1" ){
                return true;
            } else {
                return false;
            }
        }
    }

}
