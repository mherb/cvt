#include <cvt/io/xml/XMLDocument.h>
#include <cvt/util/Data.h>
#include <cvt/io/FileSystem.h>

using namespace cvt;

int main( int argc, char * argv[] )
{
	XMLDocument doc;
	if( argc == 1 ){
		String data = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" \
					   "<test bla=\"xxx\" foo='bar' />" \
					   "<xxx>test</xxx>" \
					   "<xxx><yyy>test</yyy></xxx>";
		doc.load( data.c_str(), data.length() );
		for( size_t i = 0; i < doc.nodeSize(); i++ )
			std::cout << *doc.node( i ) << std::endl;
	} else {
		Data d;
		FileSystem::load( d, argv[ 1 ], true);

		doc.load( ( const char* ) d.ptr(), d.size() );
		for( size_t i = 0; i < doc.nodeSize(); i++ )
			std::cout << *doc.node( i ) << std::endl;
		/*	XMLNode* n = doc.nodeByName("face");
			String s;
			n->xmlString( s );
			std::cout << s << std::endl;*/
	}
	return 0;
}
