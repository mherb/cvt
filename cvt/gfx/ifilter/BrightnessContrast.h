/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
