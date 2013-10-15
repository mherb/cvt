#ifndef CVT_ICONVOLVE_H
#define CVT_ICONVOLVE_H

#include <cvt/gfx/IBorder.h>
#include <cvt/gfx/Color.h>

namespace cvt
{
	class Image;
	class IKernel;

	class IConvolve {
		public:
			static void convolve( Image& dst, const Image& src, const IKernel& kernel, IBorderType btype = IBORDER_CLAMP, const Color& = Color::BLACK );
			static void convolve( Image& dst, const Image& src, const IKernel& hkernel, const IKernel& vkernel, IBorderType btype = IBORDER_CLAMP, const Color& = Color::BLACK );

		private:
			IConvolve() {}
			IConvolve( const IConvolve& ) {}
	};
}

#endif
