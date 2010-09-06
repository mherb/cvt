#ifndef CLPARAMETERINFO_H
#define CLPARAMETERINFO_H
#include <string>

namespace cvt {
	namespace cl {

		enum CLParameterType { CL_PARAM_SCALAR = 1,
			CL_PARAM_COLOR,
			CL_PARAM_IMAGE,
			CL_PARAM_VECTOR2,
			CL_PARAM_VECTOR4,
			CL_PARAM_VECTOR8,
			CL_PARAM_VECTOR16,
			CL_PARAM_BUFFER_FLOAT,
			CL_PARAM_BUFFER_FLOAT2,
			CL_PARAM_BUFFER_FLOAT4,
			CL_PARAM_BUFFER_FLOAT8,
			CL_PARAM_BUFFER_FLOAT16, };
		enum CLParameterDirection { CL_PARAM_IN = ( 1 << 0 ), CL_PARAM_OUT = ( 1 << 1 ) };


		class CLParameterInfo
		{
			public:
				CLParameterInfo( std::string n, CLParameterType t, CLParameterDirection d = CL_PARAM_IN ) : name( n ), type( t ), dir( d ) {};
				CLParameterType getType( ) const
				{
					return type;
				};
				CLParameterDirection getDirection( ) const
				{
					return dir;
				};
				std::string getName( ) const
				{
					return name;
				};

			private:
				std::string name;
				CLParameterType type;
				CLParameterDirection dir;
		};

	}
}
#endif
