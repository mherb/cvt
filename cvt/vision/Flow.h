/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_FLOW_H
#define CVT_FLOW_H

#include <cvt/gfx/Image.h>
#include <cvt/util/Exception.h>

namespace cvt {
	namespace Flow {
		float AEE( const Image& flow, const Image& gt );
		float AAE( const Image& flow, const Image& gt );
		void colorCode( Image& dst, Image& flow, float maxflow = 5.0f );
	}
}

#endif
