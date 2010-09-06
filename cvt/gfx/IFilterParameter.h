#ifndef IFILTERPARAMETER_H
#define IFILTERPARAMETER_H
#include <cvt/gfx/IFilterParameterInfo.h>
#include <string>

namespace cvt {

		class IFilterParameter
		{
			public:
				IFilterParameterType getType( ) const {	return type; };
				inline bool isType( IFilterParameterType t ) const { return type == t; };
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
