/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
