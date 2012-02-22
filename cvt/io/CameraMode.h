/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_CAMERAMODE
#define CVT_CAMERAMODE

#include <cvt/gfx/IFormat.h>
#include <cvt/util/String.h>

namespace cvt {
	struct CameraMode {
		CameraMode( size_t w = 0, size_t h = 0,
					size_t f = 0, const IFormat & pixFormat = IFormat::RGBA_UINT8 ) :
					width( w ), height( h ), fps( f ), format( pixFormat ), description( "" )
		{}

		CameraMode( const CameraMode & other ) :
			 width( other.width ), height( other.height ),
			 fps( other.fps ), format( other.format ), description( other.description )
		{
		}

		CameraMode & operator=( const CameraMode & other )
		{
			width = other.width;
			height = other.height;
			fps = other.fps;
			format = other.format;
			return *this;
		}

		bool operator==( const CameraMode& other ) const
		{
			if( width == other.width &&
			    height == other.height &&
				fps == other.fps &&
				format == other.format &&
				description == other.description )
				return true;
			return false;
		}

		size_t  width;
		size_t  height;
		size_t  fps;
		IFormat format;
		String  description;
	};

	inline std::ostream& operator<<( std::ostream &out, const CameraMode &m )
	{
		out << m.format << " (" << m.width << " x " << m.height << ") @ " << m.fps << " fps " << m.description ;
		return out;
	}

}

#endif
