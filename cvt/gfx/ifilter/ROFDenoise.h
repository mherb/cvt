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
