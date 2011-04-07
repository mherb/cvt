#ifndef CVT_XMLLEAF_H
#define CVT_XMLLEAF_H

#include <cvt/io/xml/XMLNode.h>

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
			virtual const XMLNode* child( size_t index ) const;
			virtual void addChild( XMLNode* node );
			virtual XMLNode* childByName( const String& name );
		private:
			XMLLeaf( const XMLLeaf& );
	};

	inline size_t XMLLeaf::childSize() const
	{
		return 0;
	}

	inline XMLNode* XMLLeaf::child( size_t )
	{
		return NULL;
	}

	inline const XMLNode* XMLLeaf::child( size_t ) const
	{
		return NULL;
	}


	inline void XMLLeaf::addChild( XMLNode* )
	{
	}

	inline XMLNode* XMLLeaf::childByName( const String& )
	{
		return NULL;
	}

}

#endif
