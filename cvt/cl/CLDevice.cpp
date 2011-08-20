#include <cvt/cl/CLDevice.h>
#include <cvt/cl/CLPlatform.h>

namespace cvt {

	CLPlatform CLDevice::platform() const
	{
		return CLPlatform( _devicePlatform() );
	}

	void CLDevice::parseExtensions( std::vector<String>& extensions, const char* str )
	{
		const char* sptr = str;
		const char* eptr;

		do {
			while( *sptr && *sptr == ' ' )
				sptr++;
			eptr = sptr;
			while( *eptr && *eptr != ' ' )
				eptr++;
			if( sptr != eptr ) {
				extensions.push_back( String( sptr, eptr - sptr ) );
				sptr = eptr;
			}
		} while( *eptr );

	}

	void CLDevice::extensions( std::vector<String>& extensions )
	{
		String ext;
		_extensions( ext );
		parseExtensions( extensions, ext.c_str() );
	}
}
