#ifndef IFILTER_ROFDENOISE_H
#define IFILTER_ROFDENOISE_H

#include "gfx/IFilter.h"

namespace cvt {

    class ROFDenoise : public IFilter
    {
	public:
	    void apply( Image& dst, const Image& src, float lambda = 70.0f, size_t iter = 10 );
    };
}


#endif
