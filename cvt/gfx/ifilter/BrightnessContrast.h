#ifndef IFILTER_BRIGHTNESSCOTRAST_H
#define IFILTER_BRIGHTNESSCOTRAST_H

#include <cvt/gfx/IFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class BrightnessContrast : public IFilter
	{
		public:
					BrightnessContrast();
					~BrightnessContrast();
			void	apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;

		private:
			void	applyOpenCL( Image& dst, const Image& src, float brightness, float contrast ) const;
			CLKernel _kernelBC;
	};
}

#endif
