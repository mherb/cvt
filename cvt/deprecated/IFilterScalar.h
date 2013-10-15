#ifndef IFILTERSCALAR_H
#define IFILTERSCALAR_H
#include <cvt/gfx/IFilterParameter.h>

namespace cvt {

	class IFilterScalar : public IFilterParameter
	{
		friend class CLKernel;

		public:
			IFilterScalar( float v = 0.0f ) : IFilterParameter( IFILTERPARAMETER_SCALAR ), value( v ) {};
			IFilterScalar( const IFilterScalar& ifs ) : IFilterParameter( IFILTERPARAMETER_SCALAR ) { value = ifs.value; };


			void set( float v )	{ value = v; };
			float get( void ) {	return value; };

		private:
			float value __attribute__((aligned(4)));
	};

}
#endif
