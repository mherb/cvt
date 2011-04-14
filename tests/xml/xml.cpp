#include <cvt/io/xml/XMLDocument.h>
#include <cvt/util/Data.h>
#include <cvt/io/FileSystem.h>

using namespace cvt;

int main()
{
	String data = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" \
				   "<test bla=\"xxx\" foo='bar' />" \
				   "<xxx>test</xxx>" \
				   "<xxx><yyy>test</yyy></xxx>";
	XMLDocument doc;
	doc.load( data.c_str(), data.length() );
	for( size_t i = 0; i < doc.nodeSize(); i++ )
		std::cout << *doc.node( i ) << std::endl;

	Data d;
	FileSystem::load( d, "../data/facedefault.xml");
	d.reallocate( d.size() + 1 );
	d.ptr()[ d.size() - 1 ] = '\0';

	doc.load( ( const char* ) d.ptr(), d.size() );
	for( size_t i = 0; i < doc.nodeSize(); i++ )
		std::cout << *doc.node( i ) << std::endl;
}
