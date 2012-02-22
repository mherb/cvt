/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_XMLLEAF_H
#define CVT_XMLLEAF_H

#include <cvt/io/xml/XMLNode.h>

namespace cvt {

	/**
	  \ingroup XML
	*/
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
			virtual void removeChild( XMLNode* node );
			virtual void removeChild( size_t index );
			virtual XMLNode* childByName( const String& name );
			virtual void xmlString( String& str ) const = 0;

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
			
	inline void XMLLeaf::removeChild( XMLNode* )
	{
	}

	inline void XMLLeaf::removeChild( size_t )
	{
	}

	inline XMLNode* XMLLeaf::childByName( const String& )
	{
		return NULL;
	}

}

#endif
