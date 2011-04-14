#ifndef CVT_XMLDOCUMENT_H
#define CVT_XMLDOCUMENT_H

#include <cvt/io/xml/XMLNode.h>
#include <cvt/io/xml/XMLDecoder.h>
#include <cvt/io/FileSystem.h>
#include <vector>

namespace cvt {
	class XMLDocument {
		public:
			XMLDocument();
			~XMLDocument();

			void load( const char* buffer, size_t len );
			void load( const String& path );
//			void save( const String& path );
//			void save( const char* buffer, size_t len );

			size_t nodeSize() const;
			XMLNode* node( size_t index );
			void addNode( XMLNode* node );
			XMLNode* nodeByName( const String& name );

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

	inline void XMLDocument::load( const String& path )
	{
		Data d;
		FileSystem::load( d, path.c_str(), true );
		load( ( const char* ) d.ptr(), d.size() - 1 );
	}

	inline size_t XMLDocument::nodeSize() const
	{
		return _nodes.size();
	}

	inline void XMLDocument::addNode( XMLNode* node )
	{
		_nodes.push_back( node );
	}


	inline XMLNode* XMLDocument::node( size_t index )
	{
		return _nodes[ index ];
	}

	inline XMLNode* XMLDocument::nodeByName( const String& name )
	{
		for( int i = 0, end = _nodes.size(); i < end; i++ ) {
			if( _nodes[ i ]->name() == name )
				return _nodes[ i ];
		}
		return NULL;
	}
}

#endif
