/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_THRESHOLD_H
#define CVT_THRESHOLD_H

#include <cvt/gfx/IFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class Threshold : public IFilter {
		public:
			Threshold();
			void apply( Image& dst, const Image& src, const float threshold, IFilterType = IFILTER_CPU ) const;
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;
	};
}

#endif
