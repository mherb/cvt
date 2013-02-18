/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/vision/features/ORB.h>

namespace cvt {

	const int ORB::_circularoffset[ 31 ] = {
		3,  6,  8,  9, 10, 11, 12, 13, 13, 14, 14, 14, 15, 15, 15, 15,
		15, 15, 15, 14, 14, 14, 13, 13, 12, 11, 10,  9,  8,  6,  3
	};

	#include "ORBPattern.h"
}
