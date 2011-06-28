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

namespace cvt {

    class DataIterator {
      public:

        DataIterator( Data & d );

        bool hasNext( );

        bool nextToken( String & token, const String & deliminaters );

      private:
        Data &		_data;
        uint8_t*	_pos;
        uint8_t*	_end;

        void skip( const String & delim );
        void nextDelim( const String & delim );

        bool isDeliminator( uint8_t c, const String & delims );
    } ;

    inline DataIterator::DataIterator( Data& d ) :
    	_data( d ), _pos( d.ptr( ) ), _end( _pos + d.size( ) )
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
            uint8_t * start = _pos;
            nextDelim( deliminators );

            token.assign( ( char* )start, _pos - start );
            return true;

        } else {
            return false;
        }

        return true;
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

