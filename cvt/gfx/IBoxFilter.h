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
