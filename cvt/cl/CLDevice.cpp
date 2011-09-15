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
