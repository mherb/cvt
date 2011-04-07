#include <cvt/io/xml/XMLDocument.h>

using namespace cvt;

int main()
{
	String data = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" \
				   "<test bla=\"xxx\" foo='bar' />";
	XMLDocument doc;
	doc.load( data.c_str(), data.length() );
	for( size_t i = 0; i < doc.nodeSize(); i++ )
		std::cout << *doc.node( i ) << std::endl;

}
