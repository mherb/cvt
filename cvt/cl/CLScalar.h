#ifndef CLSCALAR_H
#define CLSCALAR_H
#include <cvt/cl/CLManager.h>
#include <cvt/cl/CLParameter.h>

namespace cvt {
	namespace cl {

		class CLScalar : public CLParameter
		{
			public:
				CLScalar( CLManager* cl, float v = 0.0f );
				~CLScalar( );
				void set( float v )
				{
					value = v;
				};
				float get( void )
				{
					return value;
				};
				std::pair<size_t, void*> getCLArgument( );

				::cl::NDRange getCLNDRange( ) const;

			protected:
				CLScalar( const CLScalar& ) {};

			private:
				CLScalar( );
				void syncCL( );

				CLManager* cl;
				float value;
		};

	}
}
#endif
