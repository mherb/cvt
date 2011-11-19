#ifndef CVT_INTEGRALFILTER_H
#define CVT_INTEGRALFILTER_H

#include <cvt/gfx/IFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class IntegralFilter : public IFilter {
		public:
			IntegralFilter();
			void apply( Image& dst, const Image& src, const Image* src2 = NULL ) const;
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;

		private:
			CLKernel _clprefixsum_blockp;
			CLKernel _clprefixsum_blockp_sqr;
			CLKernel _clprefixsum_blockp_mul2;
			CLKernel _clprefixsum_horiz;
			CLKernel _clprefixsum_vert;
			CLKernel _clprefixsum_block2;
			size_t _blocksize;
	};
}

#endif
