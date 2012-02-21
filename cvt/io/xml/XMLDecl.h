/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
/*
 * File:   XMLDecl.h
 * Author: sebi
 *
 * Created on August 8, 2011, 2:50 PM
 */

#ifndef CVT_XMLDECL_H
#define	CVT_XMLDECL_H

#include <cvt/io/xml/XMLEncoding.h>
#include <cvt/io/xml/XMLLeaf.h>

namespace cvt
{

	/**
	  \ingroup XML
	*/
    class XMLDecl : public XMLLeaf
    {
      public:
        XMLDecl( XMLEncodingType encoding );

        void xmlString( String& str ) const;

      private:
        XMLEncodingType     _encoding;

    };

    inline XMLDecl::XMLDecl( XMLEncodingType encoding ) :
        XMLLeaf( XML_NODE_DECL, "xml" ),
        _encoding( encoding )
    {
    }

    inline void XMLDecl::xmlString( String& str ) const
    {
        str = "<?xml version=\"1.0\" encoding=\"";
        switch( _encoding ){
            case XMLENCODING_UTF8:
                str += "UTF-8";
            break;
        }
        str += "\"?>\n";
    }
}

#endif

