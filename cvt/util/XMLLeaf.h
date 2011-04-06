#ifndef CVT_XMLLEAF_H
#define CVT_XMLLEAF_H

#include <cvt/util/XMLNode.h>

namespace cvt {
	class XMLLeaf : public XMLNode
	{
		public:
			virtual ~XMLLeaf() {};

		protected:
			XMLLeaf( XMLNodeType type, const String& name = "", const String value = "" ) : XMLNode( type, name, value )
			{}

			virtual size_t childSize() const;
			virtual XMLNode* child( size_t index );
			virtual void addChild( const XMLNode& node );
			virtual XMLNode* childByName( const String& name );
	};

	inline size_t XMLLeaf::childSize() const
	{
		return 0;
	}

	inline XMLNode* XMLLeaf::child( size_t index )
	{
		return NULL;
	}

	inline void XMLLeaf::addChild( const XMLNode* node )
	{
	}

	inline XMLNode* XMLLeaf::childByName( const String& name )
	{
		return NULL;
	}

}

#endif
