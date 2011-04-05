#ifndef CVT_XMLATTRIBUTE_H
#define CVT_XMLATTRIBUTE_H

namespace cvt {
	class XMLAttribute {
		public:
			XMLAttribute( const String& name, const String& value );
			XMLAttribute( const XMLAttribute& other );
			XMLAttribute& operator=( const XMLAttribute& other );
			~XMLAttribute();

			const String& name() const;
			void setName( const String& name );
			const String& value() const;
			void setValue( const String& value );

		private:
			String _name;
			String _value;
	};

	inline XMLAttribute::XMLAttribute( const String& name, const String& value ) : _name( name ), _value( value )
	{
	}

	inline XMLAttribute::XMLAttribute( const XMLAttribute& other ) : _name( other._name ), _value( other._value )
	{
	}

	XMLAttribute& XMLAttribute::operator=( const XMLAttribute& other )
	{
		_name = other._name;
		_value = other._value;
		return *this;
	}

	inline const String& XMLAttribute::name() const
	{
		return _name;
	}

	inline void XMLAttribute::setName( const String& name )
	{
		_name = name;
	}

	inline const String& XMLAttribute::value() const
	{
		return _value;
	}

	inline void XMLAttribute::setValue( const String& value )
	{
		_value = value;
	}
}

#endif
