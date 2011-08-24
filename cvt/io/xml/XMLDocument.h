#ifndef CVT_XMLDOCUMENT_H
#define CVT_XMLDOCUMENT_H

#include <cvt/io/xml/XMLEncoding.h>
#include <cvt/io/xml/XMLNode.h>
#include <cvt/io/xml/XMLDecl.h>
#include <cvt/io/xml/XMLDecoder.h>
#include <cvt/io/FileSystem.h>
#include <vector>

namespace cvt {

	/**
	  \ingroup XML
	  \defgroup XML XML
	*/
	class XMLDocument {
		public:
			XMLDocument( XMLEncodingType encoding = XMLENCODING_UTF8 );
			~XMLDocument();

			void load( const char* buffer, size_t len );
			void load( const String& path );

			void save( const String& path );

//			void save( const char* buffer, size_t len );

			size_t nodeSize() const;
			XMLNode* node( size_t index );
			void addNode( XMLNode* node );
			XMLNode* nodeByName( const String& name );

		private:
			std::vector<XMLNode*> _nodes;
	};

	inline XMLDocument::XMLDocument( XMLEncodingType encoding )
	{
        XMLDecl * decl = new XMLDecl( encoding );
        addNode( decl );
	}

	inline XMLDocument::~XMLDocument()
	{

	}

	inline void XMLDocument::load( const char* buffer, size_t len )
	{
        _nodes.clear();
		XMLDecoder* dec = XMLDecoder::autodetect( buffer, len );
		dec->parse( this, buffer, len );
		delete dec;
	}

	inline void XMLDocument::load( const String& path )
	{
        _nodes.clear();
		Data d;
		FileSystem::load( d, path.c_str(), true );
		load( ( const char* ) d.ptr(), d.size() - 1 );
	}

    inline void XMLDocument::save( const String& path )
    {

        String xmlDoc;
        for( size_t i = 0; i < _nodes.size(); i++ ){
            String s;
            _nodes[ i ]->xmlString( s );
            xmlDoc += s;
        }

        Data d( ( const uint8_t* )xmlDoc.c_str(), xmlDoc.length() );

        if( !FileSystem::save( path, d ) ){
            throw CVTException( "Could not save XMLDocument" );
        }
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
