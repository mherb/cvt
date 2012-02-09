#ifndef CVT_TVL1FLOW_H
#define CVT_TVL1FLOW_H

#include <cvt/gfx/IFilter.h>
//#include <cvt/gfx/ifilter/ROFFGPFilter.h>
//#include <cvt/gfx/ifilter/GuidedFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class TVL1Flow : public IFilter {
		public:
			TVL1Flow( float scalefactor, size_t levels );
			~TVL1Flow();
			void apply( Image& flow, const Image& src1, const Image& src2 );
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const {};

		private:
			void fillPyramidCL( const Image& img, size_t index );
			void solveTVL1( Image& flow, const Image& src1, const Image& src2, bool median );

			bool		 _toggle;
			float		 _scalefactor;
			size_t		 _levels;
			CLKernel	 _pyrup;
			CLKernel	 _pyrdown;
			CLKernel	 _tvl1;
			CLKernel	 _tvl1_warp;
//			CLKernel	 _tvl1_dataadd;
			CLKernel	 _clear;
			CLKernel	 _median3;
			float		 _lambda;
//			ROFFGPFilter _rof;
//			GuidedFilter _gf;
			Image*		 _pyr[ 2 ];
	};
}

#endif
