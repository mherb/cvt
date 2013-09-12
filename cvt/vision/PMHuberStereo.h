#ifndef CVT_PMHUBERSTEREO_H
#define CVT_PMHUBERSTEREO_H

#include <cvt/gfx/Image.h>
#include <cvt/cl/CLKernel.h>

#include <cvt/gfx/PDROF.h>
#include <cvt/gfx/PDROFInpaint.h>

namespace cvt {
	class PMHuberStereo {
		public:
			PMHuberStereo();
			~PMHuberStereo();

			void depthMap( Image& dmap, const Image& left, const Image& right, size_t patchsize, float depthmax, size_t iterations, size_t viewsamples, float dscale = -1.0f );
			void depthMapInpaint( Image& dmap, const Image& left, const Image& right, size_t patchsize, float depthmax, size_t iterations, size_t viewsamples );

		private:
			CLKernel _clpmh_init;
			CLKernel _clpmh_propagate;
			CLKernel _clpmh_depthmap;
			CLKernel _clpmh_viewbufclear;
			CLKernel _clpmh_fill;
			CLKernel _clpmh_consistency;
			CLKernel _clpmh_filldepthmap;
			CLKernel _clpmh_normaldepth;
			CLKernel _clpmh_clear;
			CLKernel _clpmh_occmap;
			CLKernel _clpmh_gradxy;
			CLKernel _clpmh_weight;
			PDROF	 _pdrof;
			PDROFInpaint _pdrofinpaint;
	};

}

#endif
