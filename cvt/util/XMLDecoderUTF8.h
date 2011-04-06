#ifndef CVT_XMLDECODERUTF8_H
#define CVT_XMLDECODERUTF8_H

#include <cvt/util/XMLDecoder.h>

namespace cvt {
	class XMLDecoderUTF8 : public XMLDecoder
	{
		public:
			XMLDecoderUTF8();
			~XMLDecoderUTF8();

		protected:
			void setData( void* data, size_t len );
			XMLNode* parseDeclaration();
			XMLNode* parseNode();

		private:
			inline void skipWhitespace();
			inline void parseName( String& name );
			inline void parseAttributeValue( String& value );
			XMLNode* parseAttribute();
			XMLNode* parseComment();
			XMLNode* parseText();
			XMLNode* parseCData();

			bool match( const char* str );
			bool match( char val );
			void advance( size_t n );

			uint8_t* _base;
			size_t   _len;
			uint8_t* _stream;
			size_t	 _rlen;
			uint8_t* _end;

			enum State {
				STATE_OK,
				STATE_EOF,
			};
			State _state;
	};

	inline XMLDecoderUTF8::setData( void* data, size_t len )
	{
		_base = _stream = ( uint8_t* ) data;
		_len = _rlen = len;
		_end = stream + len;
		_state = STATE_OK;
	}

	inline XMLDecoderUTF8::skipWhitespace()
	{
		while( ( *_stream == 0x20 ||
			   *_stream == 0x09 ||
			   *_stream == 0x0A ||
			   *_stream == 0x0D ) && rlen ) {
			_stream++;
			_rlen--;
		}
	}

	inline bool XMLDecoderUTF8::match( const char* str )
	{
		const char* ptr = ( const char* ) _stream;
		size_t len = _rlen;

		while( *ptr == *str && len && *str ) {
			len--;
			str++;
			ptr++;
		}
		return *str == '\0';
	}

	inline bool XMLDecoderUTF8::match( uint8_t val )
	{
		return  *_stream == val;
	}

	inline void XMLDecoderUTF8::advance( size_t n )
	{
		_stream += n;
		if( _rlen < n ) {
			throw CVTException( "Premature end of file" )
		}
		_rlen -= n;
	}

	inline void XMLDecoderUTF8::parseName( String& name )
	{
		size_t n = 0;
		if( *_stream == ':' || *_stream == '_' ||
			   ( *_stream >= 'A' && *_stream <= 'Z' ) ||
			   ( *_stream >= 'a' && *_stream <= 'z' ) )
			n++;
		else if( *_stream >= 0x80 ) {
			
		}

		name.assign( _stream, n );
		advance( n );
	}

	inline void parseAttributeValue( String& value )
	{
		if( match('"')) {
		} else if( match('\'') ) {
		} else
			throw CVTException( "Expected attribute value" )
	}
}

#endif
