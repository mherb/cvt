#ifndef CVT_ROFFGPFILTER_H
#define CVT_ROFFGPFILTER_H

#include <cvt/gfx/IFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class ROFFGPFilter : public IFilter {
		public:
			ROFFGPFilter();
			void apply( Image& dst, const Image& src, float lambda = 70.0f, size_t iter = 10 ) const;
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;

		private:
			mutable Image _imgn;
			mutable Image _imge0;
			mutable Image _imge1;

			CLKernel _clfgpclear;
			CLKernel _clfgpcalcu;
			CLKernel _clfgpcalce;
			CLKernel _clfgpcalcn;
	};
}

#endif
