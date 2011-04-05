#ifndef CVT_XMLNODE_H
#define CVT_XMLNODE_H

#include <cvt/util/String.h>
#include <cvt/util/XMLAttribute.h>
#include <cvt/util/CVTAssert.h>
#include <vector>

namespace cvt {
	class XMLNode {
		public:
			XMLNode();
			XMLNode( const String& name );
			XMLNode( const String& name, const String& value );
			XMLNode( const XMLNode& other );
			XMLNode& operator=( const XMLNode& other );
			~XMLNode();

			const String& name();
			void setName( const String& name );

			const String& value();
			void setValue( const String& value );

			size_t childSize() const;
			XMLNode& child( size_t index );
			void addChild( const XMLNode& node );
			XMLNode* childByName( const String& name );

			size_t attributeSize() const;
			XMLAttribute& attribute( size_t index );
			void addAttribute( const XMLAttribute& attr );
			XMLAttribute* attributeByName( const String& name );

		private:
			String				 _name;
			String				 _value;
			vector<XMLAttribute> _attributes;
			vector<XMLNode>		 _children;
	};

	inline XMLNode::XMLNode()
	{
	}

	inline XMLNode::~XMLNode()
	{
	}

	inline XMLNode::XMLNode( const String& name ) : _name( name )
	{
	}

	inline XMLNode::XMLNode( const String& name, const String& value ) : _name( name ), _value( value )
	{
	}

	inline XMLNode::XMLNode( const XMLNode& other ) : _name( other._name), _value( other._value ),
													  _attributes( other._attributes ),
													  _children( other._children )
	{
	}

	inline XMLNode& XMLNode::operator=( const XMLNode& other )
	{
		_name = other._name;
		_value = other._value;
		_attributes = other._attributes;
		_children = other._children;
		return *this;
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

	inline size_t XMLNode::childSize() const
	{
		return _children.size();
	}

	inline XMLNode& XMLNode::child( size_t index )
	{
		CVTAssert( index < _children.size(), "Out of bounds!" );
		return _children[ index ];
	}

	inline void XMLNode::addChild( const XMLNode& node )
	{
		_children.push_back( node );
	}

	inline XMLNode* XMLNode::childByName( const String& name )
	{
		for( int i = 0, end = _children.size(); i < end; i++ ) {
			if( _children[ i ].name() == name )
				return &_children[ i ];
		}
		return NULL;
	}

	inline size_t XMLNode::attributeSize() const
	{
		return _attributes.size();
	}

	inline XMLAttribute& XMLNode::attribute( size_t index )
	{
		CVTAssert( index < _attributes.size(), "Out of bounds!" );
		return _attributes[ index ];
	}

	inline void XMLNode::addAttribute( const XMLAttribute& attr )
	{
		_attributes.push_back( attr );
	}

	inline XMLAttribute* attributeByName( const String& name )
	{
		for( int i = 0, end = _attributes.size(); i < end; i++ ) {
			if( _attributes[ i ].name() == name )
				return &_attributes[ i ];
		}
		return NULL;
	}

}

#endif
