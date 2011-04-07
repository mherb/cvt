#ifndef CVT_XMLNODE_H
#define CVT_XMLNODE_H

#include <cvt/util/String.h>
#include <cvt/util/CVTAssert.h>

namespace cvt {
	enum XMLNodeType {
		XML_NODE_ELEMENT,
		XML_NODE_TEXT,
		XML_NODE_CDATA,
		XML_NODE_ATTRIBUTE,
		XML_NODE_COMMENT
	};

	class XMLElement;

	class XMLNode {
		friend class XMLElement;

		public:
			virtual ~XMLNode();

//			virtual XMLNode* clone() = 0;

			const String& name() const;
			void setName( const String& name );

			const String& value() const;
			void setValue( const String& value );

			XMLNode* parent() const;

			virtual size_t childSize() const = 0;
			virtual XMLNode* child( size_t index ) = 0;
			virtual void addChild( XMLNode* node ) = 0;
			virtual XMLNode* childByName( const String& name ) = 0;

		protected:
			XMLNode( XMLNodeType type, const String& name = "", const String value = "" ) : _name( name ), _value( value ), _type( type ), _parent( NULL )
			{}

			String				 _name;
			String				 _value;
			XMLNodeType			 _type;
			XMLNode*			 _parent;

		private:
			XMLNode( const XMLNode& other );
			XMLNode& operator=( const XMLNode& other );
	};

	inline XMLNode::~XMLNode()
	{
	}

	inline XMLNode* XMLNode::parent() const
	{
		return _parent;
	}

	inline const String& XMLNode::name() const
	{
		return _name;
	}

	inline void XMLNode::setName( const String& name )
	{
		_name = name;
	}

	inline const String& XMLNode::value() const
	{
		return _value;
	}

	inline void XMLNode::setValue( const String& value )
	{
		_value = value;
	}


}

#endif
