#include <cvt/cl/CLScalar.h>

namespace cvt {
	namespace cl {

		CLScalar::CLScalar( CLManager* cl, float v ) : CLParameter( CL_PARAM_SCALAR ), value( v )
		{
			this->cl = cl;
		}

		CLScalar::~CLScalar( )
		{}

		std::pair<size_t, void*> CLScalar::getCLArgument( )
		{
			return std::make_pair<size_t, void*>( sizeof( float ), &value );
		}

		::cl::NDRange CLScalar::getCLNDRange( ) const
		{
			return ::cl::NDRange( 1 );
		}

	}
}
