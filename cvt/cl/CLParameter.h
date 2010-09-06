#ifndef CLPARAMETER_H
#define CLPARAMETER_H
#include <cvt/cl/CLManager.h>
#include <cvt/cl/CLParameterInfo.h>
#include <string>

namespace cvt {
	namespace cl {

		class CLParameter
		{
			public:
				CLParameterType getType( ) const
				{
					return type;
				};
				virtual ~CLParameter( ) {};
				inline bool isType( CLParameterType t ) const
				{
					return type == t;
				};
				virtual std::pair<size_t, void*> getCLArgument( ) = 0;

				virtual ::cl::NDRange getCLNDRange( ) const = 0;

			protected:
				CLParameter( CLParameterType t ) : type( t ) {};
				CLParameter( const CLParameter& ) {};
				CLParameter( ) {};

			private:
				CLParameterType type;
		};

	}
}

#endif
