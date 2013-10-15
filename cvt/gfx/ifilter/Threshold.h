#ifndef CVT_THRESHOLD_H
#define CVT_THRESHOLD_H

#include <cvt/gfx/IFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class Threshold : public IFilter {
		public:
			Threshold();
			void apply( Image& dst, const Image& src, const float threshold, IFilterType = IFILTER_CPU ) const;
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;
	};
}

#endif
