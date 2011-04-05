#ifndef CVT_XMLDOCUMENT_H
#define CVT_XMLDOCUMENT_H

#include <cvt/util/XMLNode.h>

namespace cvt {
	class XMLDocument {
		public:
			XMLDocument();
			void load( const String& path );
			void save( const String& path );
			void load( const char* buffer, size_t len );
			void save( const char* buffer, size_t len );

			XMLNode& root();

		private:
			XMLNode _root;
	};
}

#endif
