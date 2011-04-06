#ifndef CVT_XMLDECODER_H
#define CVT_XMLDECODER_H

namespace cvt {
	class XMLNode;

	class XMLDecoder {
		public:
			XMLDecoder() {};
			virtual ~XMLDecoder() {};

			static XMLDecoder* autodetect( void* data, size_t len );

		protected:
			void parse( XMLDocument& doc, void* data, size_t len );

			virtual void setData( void* data, size_t len );
			virtual XMLNode* parseDeclaration() = 0;
			virtual XMLNode* parseNode() = 0;

		private:
			XMLDecoder( const XMLDecoder& );
			XMLDecoder& operator=( const XMLDecoder& );
	};
}

#endif
