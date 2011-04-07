#include <cvt/io/xml/XMLDocument.h>

using namespace cvt;

int main()
{
	String data = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
	XMLDocument doc;
	doc.load( data.c_str(), data.length() );

}
