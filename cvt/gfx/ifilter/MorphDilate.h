#ifndef CVT_MORPHDILATE_H
#define CVT_MORPHDILATE_H

#include <cvt/gfx/IFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class MorphDilate : public IFilter {
		public:
			MorphDilate();
			void apply( Image& dst, const Image& src, const size_t radius, IFilterType = IFILTER_CPU ) const;
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;

		private:
			void dilateU8( Image& dst, const Image& src, size_t radius ) const;
			void dilateU16( Image& dst, const Image& src, size_t radius ) const;
			void dilateF( Image& dst, const Image& src, size_t radius ) const;
	};
}

#endif
