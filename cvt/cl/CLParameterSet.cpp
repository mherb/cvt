#include <cvt/cl/CLParameterSet.h>

namespace cvt {
	namespace cl {

		void CLParameterSet::setParameter( const std::string name, CLParameter* p )
		{
			params[ name ] = p;
		}

		CLParameterSet::CLParameterSet( const CLParameterInfo* pi, size_t n ) : pinfo( pi ), pinfosize( n )
		{
			for( size_t i = 0; i < pinfosize; i++ )
				params[ pi->getName( ) ] = 0;
		}

		CLParameterSet::~CLParameterSet( )
		{
			params.clear( );
		}

	}
}
