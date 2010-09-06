#ifndef CLCOLOR_H
#define CLCOLOR_H
#include <cvt/cl/CLManager.h>
#include <cvt/cl/CLParameter.h>

namespace cvt {
	namespace cl {

		class CLColor : public CLParameter
		{
			public:
				CLColor( CLManager* cl );
				~CLColor( );
				void set( float r, float g, float b, float a );

				void get( float& r, float& g, float& b, float& a ) const;

				std::pair<size_t, void*> getCLArgument( );

				::cl::NDRange getCLNDRange( ) const;

			protected:
				CLColor( const CLColor& ) : CLParameter( CL_PARAM_COLOR ) {};

			private:
				CLColor( );

				CLManager* cl;
				cl_float4 color;
		};
	}
}
#endif
