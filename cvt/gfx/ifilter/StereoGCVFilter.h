#ifndef CVT_STEREOGCV_H
#define CVT_STEREOGCV_H

#include <cvt/gfx/IFilter.h>
#include <cvt/gfx/ifilter/GuidedFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class StereoGCVFilter : public IFilter {
		public:
					StereoGCVFilter();
					~StereoGCVFilter();

			void	apply( Image& dst, const Image& cam0, const Image& cam1, const Matrix4f& T, float dmin, float dmax, float dt = 1.0f ) const;
			void	apply( const ParamSet* attribs, IFilterType iftype ) const {}

		private:
			CLKernel		_cldepthcost;
			CLKernel		_cldepthmin;
			CLKernel		_clfill;
			CLKernel		_clcdconv;
//			CLKernel		_cldepthrefine;
			GuidedFilter	_gf;
	};

	inline StereoGCVFilter::~StereoGCVFilter()
	{
	}
}

#endif
