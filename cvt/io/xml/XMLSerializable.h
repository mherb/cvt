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
