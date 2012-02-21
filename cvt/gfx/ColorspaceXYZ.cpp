/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/gfx/ColorspaceXYZ.h>

namespace cvt {
	const ColorspaceXYZ ColorspaceXYZ::AdobeRGB = ColorspaceXYZ( 0.64f, 0.33f, 0.21f, 0.71f, 0.15f, 0.06f, 0.3127f, 0.3290f );
	const ColorspaceXYZ ColorspaceXYZ::sRGB	    = ColorspaceXYZ( 0.64f, 0.33f, 0.30f, 0.60f, 0.15f, 0.06f, 0.3127f, 0.3290f );
	const ColorspaceXYZ ColorspaceXYZ::PALSECAM = ColorspaceXYZ( 0.64f, 0.33f, 0.29f, 0.60f, 0.15f, 0.06f, 0.3127f, 0.3290f );
}

