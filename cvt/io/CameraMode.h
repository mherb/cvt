#ifndef CVT_CAMERAMODE
#define CVT_CAMERAMODE

#include <cvt/gfx/IFormat.h>

namespace cvt {
	struct CameraMode {
		CameraMode( size_t w = 0, size_t h = 0,
					size_t f = 0, const IFormat & pixFormat = IFormat::RGBA_UINT8 ) :
					width( w ), height( h ), fps( f ), format( pixFormat )
		{}

		CameraMode( const CameraMode & other ) :
			 width( other.width ), height( other.height ),
			 fps( other.fps ), format( other.format )
		{}

		CameraMode & operator=( const CameraMode & other )
		{
			width = other.width;
			height = other.height;
			fps = other.fps;
			format = other.format;
			return *this;
		}

		size_t width;
		size_t height;
		size_t fps;
		IFormat format;
	};

	inline std::ostream& operator<<( std::ostream &out, const CameraMode &m )
	{
		out << m.format << " (" << m.width << " x " << m.height << ") @ " << m.fps << " fps";
		return out;
	}

}

#endif
