/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_XMLSERIALIZABLE_H
#define CVT_XMLSERIALIZABLE_H

namespace cvt {
	class XMLNode;

	/**
	  \ingroup XML
	*/
	class XMLSerializable {
		public:
			virtual ~XMLSerializable() {};
			virtual void	 deserialize( XMLNode* node ) = 0;
			virtual XMLNode* serialize( ) const = 0;
	};
}

#endif
