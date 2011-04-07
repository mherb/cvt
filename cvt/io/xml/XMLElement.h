#ifndef CVT_XMLELEMENT_H
#define CVT_XMLELEMENT_H

#include <cvt/io/xml/XMLNode.h>
#include <cvt/util/CVTAssert.h>
#include <vector>

namespace cvt {
	class XMLElement : public XMLNode
	{
		public:
			XMLElement( const String& name );
			~XMLElement();

			size_t childSize() const;
			XMLNode* child( size_t index );
			void addChild( XMLNode* node );
			XMLNode* childByName( const String& name );

		private:
			std::vector<XMLNode*> _children;
	};

	inline XMLElement::XMLElement( const String& name ) : XMLNode( XML_NODE_ELEMENT, name )
	{
	}

	inline XMLElement::~XMLElement()
	{
		for( int i = 0, end = _children.size(); i < end; i++ ) {
			delete _children[ i ];
		}
		_children.clear();
	}

	inline size_t XMLElement::childSize() const
	{
		return _children.size();
	}

	inline XMLNode* XMLElement::child( size_t index )
	{
		CVT_ASSERT( index < _children.size(), "Out of bounds!" );
		return _children[ index ];
	}

	inline void XMLElement::addChild( XMLNode* node )
	{
		if( !node )
			return;
		node->_parent = this;
		_children.push_back( node );
	}

	inline XMLNode* XMLElement::childByName( const String& name )
	{
		for( int i = 0, end = _children.size(); i < end; i++ ) {
			if( _children[ i ]->name() == name )
				return _children[ i ];
		}
		return NULL;
	}
}

#endif
