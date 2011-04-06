#ifndef CVT_XMLATTRIBUTE_H
#define CVT_XMLATTRIBUTE_H

#include <cvt/util/XMLNode.h>

namespace cvt {
	class XMLAttribute : public XMLNode {
		public:
			XMLAttribute( const String& name, const String& value );
			XMLAttribute( const XMLAttribute& other );
			XMLAttribute& operator=( const XMLAttribute& other );
			~XMLAttribute();

	};

	inline XMLAttribute::XMLAttribute( const String& name, const String& value ) : XMLNode( XML_NODE_ATTRIBUTE, name, value )
	{
	}

	inline XMLAttribute::XMLAttribute( const XMLAttribute& other ) : XMLNode( XML_NODE_ATTRIBUTE, other._name, other._value )
	{
	}

	inline XMLAttribute::~XMLAttribute()
	{
	}

	XMLAttribute& XMLAttribute::operator=( const XMLAttribute& other )
	{
		_name = other._name;
		_value = other._value;
		return *this;
	}
}

#endif
