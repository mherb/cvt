#ifndef IFILTER_HOMOGRAPHY_H
#define IFILTER_HOMOGRAPHY_H

#include "gfx/IFilter.h"
#include "gfx/Image.h"
#include "gfx/IFilterVector.h"

namespace cvt {

	class Homography : public IFilter
	{
		public:
			Homography();
			void apply( Image& dst, const Image& src, const IFilterVector8& h, const Color& c ) const;
			void apply( const IFilterParameterSet* set, IFilterType t = IFILTER_CPU ) const;
		private:
			void applyFloat( Image& dst, const Image& src, const IFilterVector8& h, const Color& c ) const;
	};
}


#endif
