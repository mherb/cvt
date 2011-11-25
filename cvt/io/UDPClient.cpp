#include <cvt/io/UDPClient.h>

namespace cvt
{
	UDPClient::UDPClient() : Socket( Socket::UDP_SOCKET )
	{
		bind( "0.0.0.0", 0 );
	}

	UDPClient::UDPClient( const String & addr, uint16_t port ) : Socket( Socket::UDP_SOCKET )
	{
		bind( addr, port );
	}
}
