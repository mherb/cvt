/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
