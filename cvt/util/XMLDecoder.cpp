#include <cvt/util/XMLDecoder.h>

namespace cvt{

	void XMLDecoder::parse( XMLDocument& doc, void* data, size_t len )
	{
		setData( data, len );
		XMLNode* node = parseDeclaration();
		doc.addNode( node );
		while( ( node = parseNode() ) != NULL )
			doc.addNode( node );
	}


}
