#ifndef CVT_XMLSERIALIZABLE_H
#define CVT_XMLSERIALIZABLE_H

namespace cvt {
	class XMLNode;

	class XMLSerializable {
		virtual void	 deserialize( XMLNode* node ) = 0;
		virtual XMLNode* serialize( ) = 0;
	};
}

#endif
