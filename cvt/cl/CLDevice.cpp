/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/cl/CLDevice.h>
#include <cvt/cl/CLPlatform.h>

namespace cvt {

	CLPlatform CLDevice::platform() const
	{
		return CLPlatform( _devicePlatform() );
	}

	void CLDevice::extensions( std::vector<String>& extensions ) const
	{
		String ext;
		_extensions( ext );
		ext.tokenize( extensions, ' ' );
	}
}
