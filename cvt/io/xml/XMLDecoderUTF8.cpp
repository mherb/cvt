#include <cvt/io/xml/XMLDecoderUTF8.h>
#include <cvt/util/Exception.h>
#include <cvt/io/xml/XMLElement.h>
#include <cvt/io/xml/XMLAttribute.h>

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
			throw CVTException("Malformed attribute");
		skipWhitespace();
		if( ! match( '=') )
			throw CVTException("Malformed attribute - expected '=' ");
		advance( 1 );
		skipWhitespace();
		if( !parseAttributeValue( value ) )
			throw CVTException("Malformed attribute value");
		skipWhitespace();
		std::cout << name << " = " << value << std::endl;
		return new XMLAttribute( name, value );
	}

	XMLNode* XMLDecoderUTF8::parseNode()
	{
		return NULL;
	}
}
