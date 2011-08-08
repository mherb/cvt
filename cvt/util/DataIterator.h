/*
 * File:   DataIterator.h
 * Author: Sebastian Klose
 *
 * Created on June 27, 2011
 */

#ifndef CVT_DATAITERATOR_H
#define	CVT_DATAITERATOR_H

#include <cvt/util/Data.h>
#include <cvt/util/String.h>
#include <vector>

namespace cvt {

    class DataIterator {
      public:

        DataIterator( Data & d );

        DataIterator( const String & s );

        bool hasNext( );

        bool nextToken( String & token, const String & deliminators );

        bool nextLine( String & line );

        void tokenizeNextLine( std::vector<String> & tokens, const String & deliminators );

        const uint8_t * pos() const { return _pos; }
        const uint8_t * end() const { return _end; }

        size_t remainingBytes() const { return _end - _pos; }

        long nextLong( int base = 0 );

      private:
        const uint8_t*	_pos;
        const uint8_t*	_end;

        void skip( const String & delim );
        void nextDelim( const String & delim );

        bool isDeliminator( uint8_t c, const String & delims );
    } ;

    inline DataIterator::DataIterator( Data& d ) :
    	_pos( d.ptr() ), _end( _pos + d.size() )
	{
	}

    inline DataIterator::DataIterator( const String & s ) :
        _pos( ( const uint8_t* )s.c_str() ), _end( _pos + s.length() )
    {
    }

    inline bool DataIterator::hasNext( )
    {
        return _pos != _end;
    }

    inline bool DataIterator::nextToken( String & token, const String & deliminators )
    {
        skip( deliminators );

        if ( hasNext() ) {
            // here we know that _pos points to a token
            const uint8_t * start = _pos;
            nextDelim( deliminators );

            token.assign( ( const char* )start, _pos - start );
            return true;

        } else {
            return false;
        }

        return true;
    }

    inline bool DataIterator::nextLine( String & line )
    {
        if( !hasNext() )
            return false;
        const uint8_t * start = _pos;

        while( *_pos != '\n' ){
            if( !hasNext() )
                return false;
            _pos++;
        }

        if( _pos == start ){
            // empty line
            _pos++;
            return false;
        }

        // create the line
        line.assign( ( char* )start, _pos - start );
        _pos++;
        return true;
    }

    inline void DataIterator::tokenizeNextLine( std::vector<String> & tokens, const String & deliminators )
    {
        String line;
        if( !nextLine( line ) )
            return;

        size_t start = 0;
        size_t end = line.length();
        size_t curr = 0;

        while( curr != end){
            if( isDeliminator( line[ curr ], deliminators ) ){
                curr++;
            } else {
                // curr is a non token!
                start = curr;
                while( curr != end && !isDeliminator( line[ curr ], deliminators ) )
                    curr++;
                tokens.push_back( String( &line.c_str()[ start ], curr - start ) );
            }
        }
    }

    inline long DataIterator::nextLong( int base )
    {
        if( !hasNext() ){
            throw CVTException( "No more data available" );
        }

        uint8_t * end;
        long v = strtol( ( const char* )_pos, ( char** )&end, base );

        if( *end != '\0' )
            _pos = end;
        else
            _pos = _end;

        return v;
    }

    inline bool DataIterator::isDeliminator( uint8_t c, const String & delims )
    {
        for ( size_t i = 0; i < delims.length( ); i++ ) {
            if ( delims[ i ] == c )
                return true;
        }
        return false;
    }

    inline void DataIterator::skip( const String & delim )
    {
        while ( true ) {
            if ( isDeliminator( *_pos, delim ) )
                _pos++;
            else
                return;

            if ( !hasNext( ) )
                return;
        }
    }

    inline void DataIterator::nextDelim( const String & delim )
    {
        while ( true ) {
            if ( isDeliminator( *_pos, delim )  ) {
                return;
            }
            _pos++;

            if ( !hasNext( ) )
                return;
        }
    }

}


#endif	/* DATAITERATOR_H */

