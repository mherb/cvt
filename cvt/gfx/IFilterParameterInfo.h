#ifndef IFILTERPARAMETERINFO_H
#define IFILTERPARAMETERINFO_H
#include <string>

namespace cvt {

	enum IFilterParameterType {
		IFILTERPARAMETER_SCALAR = 1,
		IFILTERPARAMETER_COLOR,
		IFILTERPARAMETER_IMAGE,
		IFILTERPARAMETER_VECTOR2,
		IFILTERPARAMETER_VECTOR4,
		IFILTERPARAMETER_VECTOR8,
		IFILTERPARAMETER_VECTOR16,
	};

	enum IFilterParameterDirection {
		IFILTERPARAMETER_IN = ( 1 << 0 ),
		IFILTERPARAMETER_OUT = ( 1 << 1 )
	};

	class IFilterParameterInfo
	{
		public:
			IFilterParameterInfo( std::string n, IFilterParameterType t, IFilterParameterDirection d = IFILTERPARAMETER_IN ) : type( t ), dir( d ), name( n ) {};

			IFilterParameterType getType( ) const { return type; };
			IFilterParameterDirection getDirection( ) const	{ return dir; };
			std::string getName( ) const { return name; };

		private:
			IFilterParameterType type;
			IFilterParameterDirection dir;
			std::string name;
	};

}
#endif
