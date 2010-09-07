#ifndef IFILTER_ROFDENOISE_H
#define IFILTER_ROFDENOISE_H

#include "gfx/IFilter.h"
#include "gfx/Image.h"

namespace cvt {

	class ROFDenoise : public IFilter
	{
		public:
			ROFDenoise();
			void apply( Image& dst, const Image& src, float lambda = 70.0f, size_t iter = 10 ) const;
			void apply( const IFilterParameterSet* set, IFilterType t = IFILTER_CPU ) const;
	};
}


#endif
