#ifndef CVT_ADAPTIVETHRESHOLD_H
#define CVT_ADAPTIVETHRESHOLD_H

#include <cvt/gfx/IFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class AdaptiveThreshold : public IFilter {
		public:
			AdaptiveThreshold();
			void apply( Image& dst, const Image& src, const Image& srcmean, const float threshold, IFilterType = IFILTER_CPU ) const;
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;
	};
}

#endif
