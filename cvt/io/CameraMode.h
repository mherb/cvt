#ifndef CVT_CAMERAMODE
#define CVT_CAMERAMODE

#include <cvt/gfx/IFormat.h>
#include <cvt/util/String.h>

namespace cvt {
	class CameraMode {
		public:
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

		public:
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
