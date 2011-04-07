#ifndef CVT_STRING_H
#define CVT_STRING_H

#include <iostream>
#include <cvt/math/Math.h>
#include <cvt/util/SIMD.h>

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
//			void split( char marker, String& out1, String& out2 ) const;
//			String operator+( const String& str ) const;
/*			String operator+( unsigned int i ) const;
			String operator+( int i ) const;
			String operator+( size_t i ) const;
			String operator+( float i ) const;
			String operator+( double i ) const;*/
			String& operator+=( const String& str );
/*			String& operator=+( unsigned int i );
			String& operator=+( int i );
			String& operator=+( size_t i );
			String& operator=+( float i );
			String& operator=+( double i );*/

			bool operator==( const String& str ) const;
			bool operator!=( const String& str ) const;
			bool hasPrefix( const String& str ) const;
			bool hasSuffix( const String& str ) const;

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
		if( _len >= _blen )
			_grow( Math::pad16( _len + 1 ) );
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

	inline String& String::operator+=( const String& str )
	{
		if( _len + str._len >= _blen )
			_grow( Math::pad16( _len + str._len + 1 ) );
		SIMD::instance()->Memcpy( ( uint8_t* ) _str + _len, ( uint8_t* ) str._str, str._len + 1 );
		_len += str._len;
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

	inline size_t String::_cstrlen( const char* str ) const
	{
		size_t ret = 0;
		while( *str++ )
			ret++;
		return ret;
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
