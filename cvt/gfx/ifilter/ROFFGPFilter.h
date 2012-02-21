/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_ROFFGPFILTER_H
#define CVT_ROFFGPFILTER_H

#include <cvt/gfx/IFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class ROFFGPFilter : public IFilter {
		public:
			ROFFGPFilter();
			void apply( Image& dst, const Image& src, float lambda = 0.1f, size_t iter = 20 ) const;
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;

		private:
			mutable Image _imge0;
			mutable Image _imge1;
			mutable Image _imge2;

			CLKernel _clfgp;
			CLKernel _clfgpdata;
			CLKernel _clfgpclear;
	};
}

#endif
