/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/io/CameraInfo.h>

namespace cvt
{
	std::ostream& operator<<( std::ostream &out, const CameraInfo &info )
	{
		static const char* _typeNames[] = {
			"Video4Linux",
			"Apple QTKit",
			"DC1394 Firewire",
			"UEYE USB",
			"OPENNI"
		};

		out << "Name: " << info.name() <<
			   ", identifier: " << info.identifier() <<
			   ", local Index: " << info.index() <<
			   ", Type: " << _typeNames[ info.type() ] << "\n";

		out << info.modeSet();

		return out;
	}

}
