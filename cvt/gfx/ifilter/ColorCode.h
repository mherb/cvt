/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_COLORCODEFILTER_H
#define CVT_COLORCODEFILTER_H

#include <cvt/gfx/IFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class ColorCode : public IFilter {
		public:
			ColorCode();
			static void apply( Image& dst, const Image& src, float min = 0.0f, float max = 100.0f );
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;
	};
}

#endif
