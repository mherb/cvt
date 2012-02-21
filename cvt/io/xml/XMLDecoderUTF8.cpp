/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/io/xml/XMLDecoderUTF8.h>
#include <cvt/util/Exception.h>
#include <cvt/io/xml/XMLElement.h>
#include <cvt/io/xml/XMLAttribute.h>
#include <cvt/io/xml/XMLComment.h>

namespace cvt {

	XMLNode* XMLDecoderUTF8::parseDeclaration()
	{
		if( ! match( "<?xml" ) )
			throw CVTException( "Invalid XML header!" );
		advance( 5 );
		XMLElement* decl = new XMLElement( "xml" );
		while( _rlen ) {
			skipWhitespace();
			if( match( "?>" ) ) {
				advance( 2 );
				break;
			} else {
				decl->addChild( parseAttribute() );
			}
		}
		return decl;
	}


	XMLNode* XMLDecoderUTF8::parseAttribute()
	{
		String name, value ;

		if( !parseName( name ) )
			throw CVTException("Malformed attribute name");
		skipWhitespace();
		if( ! match( '=') )
			throw CVTException("Malformed attribute - expected '='");
		advance( 1 );
		skipWhitespace();
		if( !parseAttributeValue( value ) )
			throw CVTException("Malformed attribute value");
		skipWhitespace();
		return new XMLAttribute( name, value );
	}

	XMLNode* XMLDecoderUTF8::parseNode()
	{
		skipWhitespace();

		if( !_rlen || match( ( uint8_t ) 0 ) )
			return NULL;

		if( match("<?") )
			return parsePI();
		if( match("<!--") )
			return parseComment();
		if( match("<") )
			return parseElement();
		else {
			std::cout << "'" << *_stream << "'" << std::endl;
			std::cout << "'" << *( _stream + 1 ) << "'" << std::endl;
			throw CVTException("Invalid XML data");
		}

		if( !match('<') )
			throw CVTException("Malformed element - expected \"<\"");
		advance( 1 );
	}

	XMLNode* XMLDecoderUTF8::parseElement()
	{
		String name;
		advance(1);
		/* spec says no whitespaces allowd - whatever */
		skipWhitespace();
		if(!parseName( name))
			throw CVTException("Malformed element name");

		/* START Tag */
		XMLElement* element = new XMLElement( name );
		while( _rlen ) {
			skipWhitespace();
			if( match( "/>" ) ) {
				advance( 2 );
				return element;
			} else if( match( '>' ) )	{
				advance( 1 );
				break;
			} else {
				element->addChild( parseAttribute() );
			}
		}

		/* Content */
		 skipWhitespace();
		 while( !match("</") ) {
			if( match("<?") )
				element->addChild( parsePI() );
			else if( match("<!--") )
				element->addChild( parseComment() );
			else if( match("<") )
				element->addChild( parseElement() );
			else if( match("<![CDATA[") )
				element->addChild( parseCData() );
			else
				element->addChild( parseText() );
			skipWhitespace();
		}

		/* END Tag */
		advance( 2 );
		/* spec says no whitespaces allowd - whatever */
		skipWhitespace();
		String ename;
		if( ! parseName( ename ) )
			throw CVTException("Malformed element name");
		if( name != ename )
			throw CVTException("Names in start- and end-tag differ");
		skipWhitespace();
		if( !match('>') )
			throw CVTException("Missing '>'");
		advance( 1 );
		return element;
	}

	XMLNode* XMLDecoderUTF8::parsePI()
	{
		/* FIXME */
		return NULL;
	}

	XMLNode* XMLDecoderUTF8::parseCData()
	{
		/* FIXME */
		return NULL;
	}

	XMLNode* XMLDecoderUTF8::parseComment()
	{
		size_t n = 0;
		size_t len;
		const uint8_t* ptr;
		String comment;

		/* discard "<!--" */
		advance(4);

		ptr = _stream;
		len = _rlen;
		while( *ptr != 0 && len ) {
			if( *ptr == '-' ) {
				if( match( ( const char* ) ptr, "-->") ) {
					comment.assign( ( const char* )  _stream, n );
					advance( n + 3 );
					return new XMLComment( comment );
				}
			}
			len--;
			n++;
			ptr++;
		}
		throw CVTException("Invalid comment");
	}
}
