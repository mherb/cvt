#include <cvt/io/TCPClient.h>

namespace cvt
{
	TCPClient::TCPClient() : Socket( Socket::TCP_SOCKET )
	{
	}

	TCPClient::TCPClient( int fd ) :
		Socket( Socket::TCP_SOCKET, fd )
	{
	}
}
