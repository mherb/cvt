#include <CLDevice.h>
#include <CLPlatform.h>

namespace cvt {

	CLPlatform CLDevice::platform() const
	{
		return CLPlatform( devicePlatform() );
	}
}
