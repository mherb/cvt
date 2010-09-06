#ifndef IFILTERSCALAR_H
#define IFILTERSCALAR_H
#include <cvt/gfx/IFilterParameter.h>

namespace cvt {

	class IFilterScalar : public IFilterParameter
	{
		public:
			IFilterScalar( float v = 0.0f ) : value( v ) {};
			IFilterScalar( const IFilterScalar& ifs ) { value = ifs.value; };

			void set( float v )	{ value = v; };
			float get( void ) {	return value; };

		private:
			float value;
	};

}
#endif
