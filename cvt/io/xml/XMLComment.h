/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_XMLCOMMENT_H
#define CVT_XMLCOMMENT_H

#include <cvt/io/xml/XMLLeaf.h>

namespace cvt {

	/**
	  \ingroup XML
	*/
	class XMLComment : public XMLLeaf {
		public:
			XMLComment( const String& value );
			XMLComment( const XMLComment& other );
			XMLComment& operator=( const XMLComment& other );
			~XMLComment();

			void xmlString( String& str ) const;
	};

	inline XMLComment::XMLComment( const String& value ) : XMLLeaf( XML_NODE_COMMENT, "", value )
	{
	}

	inline XMLComment::XMLComment( const XMLComment& other ) : XMLLeaf( XML_NODE_COMMENT, "", other._value )
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


	void XMLComment::xmlString( String& str ) const
	{
		str = "<!--";
		str += _value;
		str += "-->\n";
	}
}

#endif
