#ifndef CVT_INTEGRALFILTER_H
#define CVT_INTEGRALFILTER_H

#include <cvt/gfx/IFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class GuidedFilter;
	class StereoGCVFilter;
	class IntegralFilter : public IFilter {
		friend class GuidedFilter;
		friend class StereoGCVFilter;
		public:
			IntegralFilter();
			void apply( Image& dst, const Image& src, const Image* src2 = NULL ) const;
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;

		private:
			void applyOuterRGB( Image& dst_RR_RG_RB, Image& dst_GG_GB_BB, const Image& src ) const;

			CLKernel _clprefixsum_blockp;
			CLKernel _clprefixsum_blockp_sqr;
			CLKernel _clprefixsum_blockp_mul2;
			CLKernel _clprefixsum_blockp_outerrgb;
			CLKernel _clprefixsum_horiz;
			CLKernel _clprefixsum_vert;
			CLKernel _clprefixsum_block2;
			size_t _blocksize;
	};
}

#endif
