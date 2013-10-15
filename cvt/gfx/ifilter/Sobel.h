/*
   The MIT License (MIT)

   Copyright (c) 2011 - 2013, Philipp Heise and Sebastian Klose

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

/*
 *  Sobel.h
 *  CVTools
 *
 *  Created by Sebastian Klose on 08.09.10.
 *  Copyright 2010 sebik.org. All rights reserved.
 *
 */

#ifndef CVT_SOBEL_H
#define CVT_SOBEL_H

#include "gfx/IFilter.h"
#include "gfx/Image.h"

namespace cvt {
	
	class Sobel : public IFilter
	{		
	public:
		Sobel();
		
		// calc dx, dy and gradient magnitude image 
		// from input grayscale image
		void apply(Image & dx, Image & dy, const Image & srcGray, bool normalize = false) const;
		
		void apply( const IFilterParameterSet* set, IFilterType t = IFILTER_CPU ) const;
		
		static void nonMaximalSuppression( Image & nonMaxSuppressed, const Image & dx, const Image & dy, const Image & magnitude );		
		
		static void magnitude( Image & magnitude, const Image & dx, const Image & dy );
		
	private:
		Image kernelDx;
		Image kernelDy;
		

	};
	
}

#endif

