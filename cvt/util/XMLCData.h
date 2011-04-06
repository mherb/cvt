#ifndef CVT_XMLCDATA_H
#define CVT_XMLCDATA_H

#include <cvt/util/XMLNode.h>

namespace cvt {
	class XMLCData : public XMLNode {
		public:
			XMLCData( const String& value );
			XMLCData( const XMLCData& other );
			XMLCData& operator=( const XMLCData& other );
			~XMLCData();

	};

	inline XMLCData::XMLCData( const String& value ) : XMLNode( XML_NODE_CDATA, "", value )
	{
	}

	inline XMLCData::XMLCData( const XMLCData& other ) : XMLNode( XML_NODE_CDATA, "", other._value )
	{
	}

	inline XMLCData::~XMLCData()
	{
	}

	XMLCData& XMLCData::operator=( const XMLCData& other )
	{
		_name = "";
		_value = other._value;
		return *this;
	}
}

#endif
