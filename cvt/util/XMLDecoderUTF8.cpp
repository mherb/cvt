#include <cvt/util/XMLDecoderUTF8.h>
#include <cvt/util/Exception.h>

namespace cvt {

	XMLNode* XMLDecoderUTF8::parseDeclaration()
	{
		if( ! match( "<?xml" ) )
			throw CVTException( "Invalid XML header!" );
		advance( 5 );
		XMLElement* decl = new XMLNode( "xml" );
		while( _rlen ) {
			skipWhitespace();
			if( match( "?>" ) ) {
				advance( 2 );
			} else {
				decl->addChild( parseAttribute() );
			}
		}
		return decl;
	}


	XMLNode* XMLDecoderUTF8::parseAttribute()
	{
		String name, value ;

		parseName( name );
		skipWhitespace();
		if( ! match( '=') )
			throw CVTException("Expected '=' ... ");
		advance( 1 );
		skipWhitespace();
		parseAttributeValue( value );
		return new XMLAttribute( name, value );
	}
}
