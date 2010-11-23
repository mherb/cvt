#include <cvt/io/CameraInfo.h>

namespace cvt
{
	std::ostream& operator<<( std::ostream &out, const CameraInfo &info )
	{
		static const char* _typeNames[] = {
			"Video4Linux",
			"Apple QTKit",
			"DC1394 Firewire",
			"IDS ueye"
		};

		out << "Name: " << info.name() <<
			   ", local Index: " << info.index() <<
			   ", Type: " << _typeNames[ info.type() ] << "\n";

		out << info.modeSet();

		return out;
	}

}
