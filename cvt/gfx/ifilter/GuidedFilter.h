#ifndef CVT_GUIDEDFILTER_H
#define CVT_GUIDEDFILTER_H

#include <cvt/gfx/IFilter.h>
#include <cvt/util/Plugin.h>
#include <cvt/util/PluginManager.h>
#include <cvt/cl/CLKernel.h>

#include <cvt/gfx/ifilter/IntegralFilter.h>
#include <cvt/gfx/ifilter/BoxFilter.h>

namespace cvt {
	class GuidedFilter : public IFilter {
		public:
			GuidedFilter();
			~GuidedFilter() {};

			void apply( Image& dst, const Image& src, const Image& guide, const int radius, const float epsilon, bool rgbcovariance = false ) const;

			void apply( const ParamSet* attribs, IFilterType iftype ) const;

		private:
			void applyGC( Image& dst, const Image& src, const Image& guide, const int radius, const float epsilon ) const;
			void applyGC_COV( Image& dst, const Image& src, const Image& guide, const int radius, const float epsilon ) const;
			void applyCC( Image& dst, const Image& src, const Image& guide, const int radius, const float epsilon ) const;

			GuidedFilter( const GuidedFilter& t );

			CLKernel	   _clguidedfilter_calcab;
			CLKernel	   _clguidedfilter_calcab_outerrgb;
			CLKernel	   _clguidedfilter_applyab_gc;
			CLKernel	   _clguidedfilter_applyab_gc_outer;
			CLKernel	   _clguidedfilter_applyab_cc;
			IntegralFilter _intfilter;
			BoxFilter	   _boxfilter;
	};
}

#endif
