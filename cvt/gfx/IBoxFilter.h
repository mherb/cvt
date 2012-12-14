/*
			CVT - Computer Vision Tools Library

	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
	PARTICULAR PURPOSE.
 */
#ifndef CVT_IBOXFILTER_H
#define CVT_IBOXFILTER_H

#include <string.h>

namespace cvt
{
	class Image;

	class IBoxFilter {
		public:
			static void boxfilter( Image& dst, const Image& src, size_t radiushorizontal, size_t radiusvertical );

		private:
			IBoxFilter() {}
			IBoxFilter( const IBoxFilter& ) {}
	};
}

#endif
