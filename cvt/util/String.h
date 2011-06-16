#ifndef CVT_STRING_H
#define CVT_STRING_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <cvt/math/Math.h>
#include <cvt/util/SIMD.h>
#include <cvt/util/Exception.h>

namespace cvt {
	class String {
		friend std::ostream& operator<<( std::ostream &out, const String& str );

		public:
			String();
			String( const char* str );
			String( const String& str );
			explicit String( const char* str, size_t size );
			explicit String( const String& str, int start, int len = -1 );
			~String();

			size_t length() const;

			char operator[]( int i ) const;

			String substring( int start, int len = - 1 ) const;
			void truncate( size_t newlen );
			void assign( const char* str, size_t len );
			void sprintf( const char* format, ... );
			void sprintfConcat( const char* format, ... );
//			void split( char marker, String& out1, String& out2 ) const;
//			String operator+( const String& str ) const;
/*			String operator+( unsigned int i ) const;
			String operator+( int i ) const;
			String operator+( size_t i ) const;
			String operator+( float i ) const;
			String operator+( double i ) const;*/
			String& operator=( const String& str );
			String& operator=( const char* str );

			String& operator+=( const String& str );
//			String& operator+=( unsigned int i );
			String& operator+=( int i );
			String& operator+=( size_t i );
			String& operator+=( float i );
			String& operator+=( double i );

			bool operator==( const String& str ) const;
			bool operator!=( const String& str ) const;
			bool operator<( const String& str ) const;
			bool operator>( const String& str ) const;
			bool hasPrefix( const String& str ) const;
			bool hasSuffix( const String& str ) const;
			bool isEmpty() const;

			ssize_t find( char c, ssize_t pos = 0 ) const;
			ssize_t rfind( char c, ssize_t pos = -1 ) const;

			long int toInteger() const;
			float	 toFloat() const;
			double	 toDouble() const;

			const char* c_str() const;
		private:
			size_t _cstrlen( const char* str ) const;
			void   _grow( size_t newsize );

			char* _str;
			size_t _len;
			size_t _blen;
	};

	inline String::String()
	{
		_str = new char[ 1 ];
		_len = 0;
		_blen = 1;
		_str[ _len ] = 0;
	}

	inline String::~String()
	{
		delete[] _str;
	}

	inline String::String( const char* str )
	{
		_len = _cstrlen( str );
		_blen = Math::pad16( _len + 1 );
		_str = new char[ _blen ];
		SIMD::instance()->Memcpy( ( uint8_t* ) _str, ( uint8_t* ) str, _len + 1 );
	}

	inline String::String( const char* str, size_t len )
	{
		_len = Math::min( _cstrlen( str ), len );
		_blen = Math::pad16( _len + 1 );
		_str = new char[ _blen ];
		SIMD::instance()->Memcpy( ( uint8_t* ) _str, ( uint8_t* ) str, _len );
		_str[ _len ] = 0;
	}

	inline String::String( const String& str )
	{
		_len  = str._len;
		_blen = str._blen;
		_str = new char[ _blen ];
		SIMD::instance()->Memcpy( ( uint8_t* ) _str, ( uint8_t* ) str._str, _len + 1 );
	}

	inline String::String( const String& str, int start, int len )
	{
		if( len > 0 && ( size_t ) start + len <= str._len  ) {
			_len  = len;
			_blen = Math::pad16( _len + 1 );
			_str = new char[ _blen ];
			SIMD::instance()->Memcpy( ( uint8_t* ) _str, ( uint8_t* ) str._str + start, _len );
		} else {
			if( ( size_t ) start > str._len || len == 0 ) {
				_str = new char[ 1 ];
				_len = 0;
				_blen = 1;
			} else {
				_len  = str._len - start;
				_blen = Math::pad16( _len + 1 );
				_str = new char[ _blen ];
				SIMD::instance()->Memcpy( ( uint8_t* ) _str, ( uint8_t* ) str._str + start, _len );
			}
		}
		_str[ _len ] = 0;
	}

	inline size_t String::length() const
	{
		return _len;
	}

	inline char String::operator[]( int i ) const
	{
		return *( _str + i );
	}

	inline const char* String::c_str() const
	{
		return _str;
	}

	inline void String::assign( const char* str, size_t len )
	{
		_len = len;
		if( _len >= _blen ) {
			delete[] _str;
			_blen = Math::pad16( _len + 1 );
			_str = new char[ _blen ];
		}
		SIMD::instance()->Memcpy( ( uint8_t* ) _str, ( uint8_t* ) str, _len );
		_str[ _len ] = 0;
	}

	inline String String::substring( int start, int len ) const
	{
		return String( *this, start, len );
	}

	inline void String::truncate( size_t newlen )
	{
		if( newlen >= _len )
			return;
		_len = newlen;
		_str[ _len ] = 0;
	}

	inline String& String::operator=( const String& str )
	{
		if( str._len >= _blen )
			_grow( Math::pad16( str._len + 1 ) );
		SIMD::instance()->Memcpy( ( uint8_t* ) _str, ( uint8_t* ) str._str, str._len + 1 );
		_len = str._len;
		return *this;
	}


	inline String& String::operator=( const char* str )
	{
		size_t nlen = _cstrlen( str );
		if( nlen >= _blen )
			_grow( Math::pad16( nlen + 1 ) );
		SIMD::instance()->Memcpy( ( uint8_t* ) _str, ( uint8_t* ) str, nlen + 1 );
		_len = nlen;
		return *this;
	}

	inline String& String::operator+=( const String& str )
	{
		if( _len + str._len >= _blen )
			_grow( Math::pad16( _len + str._len + 1 ) );
		SIMD::instance()->Memcpy( ( uint8_t* ) ( _str + _len ), ( uint8_t* ) str._str, str._len + 1 );
		_len += str._len;
		return *this;
	}

/*
	inline String& String::operator+=( unsigned int i )
	{
		sprintfConcat( "%u", i );
		return *this;
	}
*/

	inline String& String::operator+=( int i )
	{
		sprintfConcat( "%d", i );
		return *this;
	}

	inline String& String::operator+=( size_t i )
	{
		sprintfConcat( "%zd", i );
		return *this;
	}

	inline String& String::operator+=( float f )
	{
		sprintfConcat( "%f", f );
		return *this;
	}

	inline String& String::operator+=( double d )
	{
		sprintfConcat( "%f", d );
		return *this;
	}


	inline bool String::operator==( const String& str ) const
	{
		if( _len != str._len )
			return false;

		const char* ptr1 = _str;
		const char* ptr2 = str._str;
		size_t len = _len;

		while( *ptr1++ == *ptr2++ && len )
			len--;
		return len?false:true;
	}


	inline bool String::operator!=( const String& str ) const
	{
		if( _len != str._len )
			return true;

		const char* ptr1 = _str;
		const char* ptr2 = str._str;
		size_t len = _len;

		while( len && *ptr1++ == *ptr2++ )
			len--;
		return len?true:false;
	}

	inline bool String::hasPrefix( const String& str ) const
	{
		if( str._len > _len )
			return false;

		const char* ptr1 = _str;
		const char* ptr2 = str._str;
		size_t len = str._len;

		while( len && *ptr1++ == *ptr2++ )
			len--;
		return len?false:true;
	}

	inline bool String::operator<( const String& str ) const
	{
		const char* ptr1 = _str;
		const char* ptr2 = str._str;
		size_t len = str._len;

		while( len && *ptr1 == *ptr2 ) {
			ptr1++;
			ptr2++;
			len--;
		}
		if( !len )
			return false;
		if( *ptr1 < *ptr2 )
			return true;
		return false;
	}

	inline bool String::operator>( const String& str ) const
	{
		const char* ptr1 = _str;
		const char* ptr2 = str._str;
		size_t len = str._len;

		while( len && *ptr1 == *ptr2 ) {
			ptr1++;
			ptr2++;
			len--;
		}
		if( !len )
			return false;
		if( *ptr1 > *ptr2 )
			return true;
		return false;
	}

	inline bool String::hasSuffix( const String& str ) const
	{
		if( str._len > _len )
			return false;

		const char* ptr1 = _str + _len - str._len;
		const char* ptr2 = str._str;
		size_t len = str._len;

		while( len && *ptr1++ == *ptr2++ )
			len--;
		return len?false:true;
	}

	inline bool String::isEmpty() const
	{
		return _len == 0;
	}

	inline ssize_t String::find( char c, ssize_t pos ) const
	{
		if( pos < 0 || pos >= ( ssize_t ) _len )
			return -1;

		ssize_t n = _len - pos;
		char* ptr = _str + pos;
		while( *ptr != c && n ) {
			ptr++;
			n--;
		}
		if( *ptr != c )
			return -1;
		return ptr - _str;
	}

	inline ssize_t String::rfind( char c, ssize_t pos ) const
	{
		if( pos >= ( ssize_t ) _len || pos == 0 )
			return -1;

		if( pos < 0 )
			pos = _len - 1;

		ssize_t n = pos;
		char* ptr = _str + pos;
		while( *ptr != c && n ) {
			ptr--;
			n--;
		}
		if( *ptr != c )
			return -1;
		return ptr - _str;
	}


	inline long int String::toInteger() const
	{
		return strtol( _str, NULL, 0 );
	}

	inline float String::toFloat() const
	{
		return strtof( _str, NULL );
	}

	inline double String::toDouble() const
	{
		return strtod( _str, NULL );
	}

	inline void String::sprintf( const char* format, ... )
	{
		size_t n;
		va_list args;

		va_start( args, format );
		n = vsnprintf( _str, _blen - 1, format, args );
		va_end( args );
		if( n < _blen ) {
			_len = n;
			return;
		}

		/* try again with more space and delete old buffer */
		delete[] _str;
		_str = NULL;
		_grow( Math::pad16( n + 1 ) );

		va_start( args, format );
		n = vsnprintf( _str, _blen - 1, format, args );
		va_end( args );
		if( n < _blen ) {
			_len = n;
			return;
		}
		throw CVTException("Error in String::sprintf");
	}

	inline void String::sprintfConcat( const char* format, ... )
	{
		size_t n;
		va_list args;

		va_start( args, format );
		n = vsnprintf( _str + _len, _blen - _len - 1, format, args );
		va_end( args );
		if( n < _blen - _len ) {
			_len += n;
			return;
		}

		/* try again with more space and keep old buffer contents*/
		_grow( Math::pad16( _blen + n + 1 ) );

		va_start( args, format );
		n = vsnprintf( _str + _len, _blen - _len - 1, format, args );
		va_end( args );
		if( n < _blen - _len ) {
			_len += n;
			return;
		}
		throw CVTException("Error in String::sprintfConcat");
	}

	inline size_t String::_cstrlen( const char* str ) const
	{
		const char* ptr;
		for( ptr = str; *ptr; ++ptr )
			;
		return ( ptr - str );
	}

	inline void String::_grow( size_t newsize )
	{
		char* newstr = new char[ newsize ];
		if( _str ) {
			SIMD::instance()->Memcpy( ( uint8_t* ) newstr, ( uint8_t* ) _str, _len + 1 );
			delete[] _str;
		}
		_str = newstr;
		_blen = newsize;
	}

	inline std::ostream& operator<<( std::ostream &out, const String& str )
	{
		return out << str._str;
	}
};

#endif
