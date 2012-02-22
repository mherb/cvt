/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
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

