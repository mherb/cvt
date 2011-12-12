#ifndef CVT_TVL1FLOW_H
#define CVT_TVL1FLOW_H

#include <cvt/gfx/IFilter.h>
#include <cvt/gfx/ifilter/ROFFGPFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class TVL1Flow : public IFilter {
		public:
			TVL1Flow( float scalefactor, size_t levels );
			void apply( Image& flow, const Image& src ) const;
//			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;

		private:
			float		 _scalefactor;
			size_t		 _levels;
			CLKernel	 _pyrup;
			CLKernel	 _pyrdown;
			CLKernel	 _flowthreshold;
			ROFFGPFilter _rof;
			Image*		 _pyr[ 2 ];
	};
}

#endif
