#ifndef IFILTERPARAMETER_H
#define IFILTERPARAMETER_H
#include <cvt/gfx/IFilterParameterInfo.h>
#include <string>

namespace cvt {

		class IFilterParameter
		{
			public:
				IFilterParameterType getIFilterParameterType( ) const {	return type; };
				inline bool isIFilterParameterType( IFilterParameterType t ) const { return type == t; };
				virtual ~IFilterParameter( ) {};

			protected:
				IFilterParameter( IFilterParameterType t ) : type( t ) {};
				IFilterParameter( const IFilterParameter& ) {};
				IFilterParameter( ) {};

			private:
				IFilterParameterType type;
		};

}

#endif
