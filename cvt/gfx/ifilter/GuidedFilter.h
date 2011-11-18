#ifndef CVT_GUIDEDFILTER_H
#define CVT_GUIDEDFILTER_H

#include <cvt/gfx/IFilter.h>
#include <cvt/util/Plugin.h>
#include <cvt/util/PluginManager.h>
#include <cvt/cl/CLKernel.h>

#include <gfx/ifilter/IntegralFilter.h>
#include <gfx/ifilter/BoxFilter.h>

namespace cvt {
	class GuidedFilter : public IFilter {
		public:
			GuidedFilter();
			~GuidedFilter() {};

			void apply( Image& dst, const Image& src, const Image& guide, const int radius, const float epsilon ) const;

			void apply( const ParamSet* attribs, IFilterType iftype ) const;

		private:
			GuidedFilter( const GuidedFilter& t );

			CLKernel	   _clguidedfilter_calcab;
			CLKernel	   _clguidedfilter_applyab;
			IntegralFilter _intfilter;
			BoxFilter	   _boxfilter;
	};
}

#endif
