#ifndef CVT_XMLTEXT_H
#define CVT_XMLTEXT_H

#include <cvt/io/xml/XMLNode.h>

namespace cvt {
	class XMLText : public XMLNode {
		public:
			XMLText( const String& value );
			XMLText( const XMLText& other );
			XMLText& operator=( const XMLText& other );
			~XMLText();

	};

	inline XMLText::XMLText( const String& value ) : XMLNode( XML_NODE_TEXT, "", value )
	{
	}

	inline XMLText::XMLText( const XMLText& other ) : XMLNode( XML_NODE_TEXT, "", other._value )
	{
	}

	inline XMLText::~XMLText()
	{
	}

	XMLText& XMLText::operator=( const XMLText& other )
	{
		_name = "";
		_value = other._value;
		return *this;
	}
}

#endif
