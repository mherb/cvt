#ifndef IFILTER_HOMOGRAPHY_H
#define IFILTER_HOMOGRAPHY_H

#include <cvt/gfx/IFilter.h>
#include <cvt/gfx/Image.h>


namespace cvt {

	class Homography : public IFilter
	{
		public:
			Homography();
			void apply( Image& dst, const Image& src, const Matrix3f& H, const Color& c ) const;
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;
		private:
			void applyFloat( Image& dst, const Image& src, const Matrix3f& H, const Color& c ) const;
	};
}


#endif
