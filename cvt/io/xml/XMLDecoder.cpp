/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/io/xml/XMLDecoder.h>
#include <cvt/io/xml/XMLDocument.h>
#include <cvt/io/xml/XMLDecoderUTF8.h>

namespace cvt{

	XMLDecoder* XMLDecoder::autodetect( const void* data, size_t len )
	{
		return new XMLDecoderUTF8();
	}

	void XMLDecoder::parse( XMLDocument* doc, const void* data, size_t len )
	{
		setData( data, len );
		XMLNode* node = parseDeclaration();
		doc->addNode( node );
		while( ( node = parseNode() ) != NULL )
			doc->addNode( node );
	}


}
