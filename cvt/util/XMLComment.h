#ifndef CVT_XMLCOMMENT_H
#define CVT_XMLCOMMENT_H

#include <cvt/util/XMLNode.h>

namespace cvt {
	class XMLComment : public XMLNode {
		public:
			XMLComment( const String& name, const String& value );
			XMLComment( const XMLComment& other );
			XMLComment& operator=( const XMLComment& other );
			~XMLComment();

	};

	inline XMLComment::XMLComment( const String& name, const String& value ) : XMLNode( XML_NODE_COMMENT, name, value )
	{
	}

	inline XMLComment::XMLComment( const XMLComment& other ) : XMLNode( XML_NODE_COMMENT, other._name, other._value )
	{
	}

	inline XMLComment::~XMLComment()
	{
	}

	XMLComment& XMLComment::operator=( const XMLComment& other )
	{
		_name = other._name;
		_value = other._value;
		return *this;
	}
}

#endif
