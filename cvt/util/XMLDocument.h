#ifndef CVT_XMLDOCUMENT_H
#define CVT_XMLDOCUMENT_H

#include <cvt/util/XMLNode.h>
#include <vector>

namespace cvt {
	class XMLDocument {
		public:
			XMLDocument();
			~XMLDocument();

			void load( const String& path );
			void save( const String& path );
			void load( const char* buffer, size_t len );
			void save( const char* buffer, size_t len );

			size_t nodeSize() const;
			XMLNode* node( size_t index );
			void addNode( XMLNode* node );

		private:
			vector<XMLNode*> _nodes;
	};
}

#endif
