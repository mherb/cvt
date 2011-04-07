#ifndef CVT_XMLDOCUMENT_H
#define CVT_XMLDOCUMENT_H

#include <cvt/io/xml/XMLNode.h>
#include <cvt/io/xml/XMLDecoder.h>
#include <vector>

namespace cvt {
	class XMLDocument {
		public:
			XMLDocument();
			~XMLDocument();

//			void load( const String& path );
//			void save( const String& path );
			void load( const char* buffer, size_t len );
//			void save( const char* buffer, size_t len );

			size_t nodeSize() const;
			XMLNode* node( size_t index );
			void addNode( XMLNode* node );

		private:
			std::vector<XMLNode*> _nodes;
	};

	inline XMLDocument::XMLDocument()
	{
	}

	inline XMLDocument::~XMLDocument()
	{

	}

	inline void XMLDocument::load( const char* buffer, size_t len )
	{
		XMLDecoder* dec = XMLDecoder::autodetect( buffer, len );
		dec->parse( this, buffer, len );
		delete dec;
	}

	inline size_t XMLDocument::nodeSize() const
	{
		return _nodes.size();
	}

	inline void XMLDocument::addNode( XMLNode* node )
	{
		_nodes.push_back( node );
	}
}

#endif
