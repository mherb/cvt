#ifndef CVT_BOXFILTER_H
#define CVT_BOXFILTER_H

#include <cvt/gfx/IFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class BoxFilter : public IFilter {
		public:
			BoxFilter();
			void apply( Image& dst, const Image& src, const int r  ) const;
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;

		private:
			CLKernel _clprefixsum_boxfilter;
	};
}

#endif
