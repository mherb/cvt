#ifndef CVT_COLORCODEFILTER_H
#define CVT_COLORCODEFILTER_H

#include <cvt/gfx/IFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class ColorCode : public IFilter {
		public:
			ColorCode();
			void apply( Image& dst, const Image& src, float min = 0.0f, float max = 100.0f ) const;
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;
	};
}

#endif
