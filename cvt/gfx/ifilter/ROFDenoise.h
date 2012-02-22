/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef IFILTER_ROFDENOISE_H
#define IFILTER_ROFDENOISE_H

#include <cvt/gfx/IFilter.h>
#include <cvt/gfx/Image.h>

namespace cvt {

	class ROFDenoise : public IFilter
	{
		public:
			ROFDenoise();
			void apply( Image& dst, const Image& src, float lambda = 70.0f, uint64_t iter = 10 ) const;
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;
	};
}


#endif
