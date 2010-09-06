#ifndef IFILTER_H
#define IFILTER_H
#include <stdint.h>
#include <string>
#include "gfx/IFilterParameterSet.h"

namespace cvt {
	enum IFilterType {
					   IFILTER_CPU = ( 1 << 0 ),
					   IFILTER_OPENGL = ( 1 << 1 ),
					   IFILTER_OPENCL = ( 1 << 2 )
					 };

	class IFilter {
		public:
			virtual IFilterParameterSet* getAttributes() const { return new IFilterParameterSet( _pinfo, _pinfosize ); };
			virtual void apply( const IFilterParameterSet* attribs, IFilterType iftype = IFILTER_CPU ) = 0;

		protected:
			IFilter( std::string name, IFilterParameterInfo const* pinfo, size_t pinfosize, uint32_t ifiltertype ) : _iftype( ifiltertype ), _name( name ), _pinfo( pinfo), _pinfosize( pinfosize) {};

			uint32_t _iftype;
			std::string _name;
			IFilterParameterInfo const* _pinfo;
			size_t _pinfosize;
	};

}

#endif
