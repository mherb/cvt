#include <cvt/com/TCPServer.h>
#include <cvt/com/TCPClient.h>

namespace cvt
{
	TCPServer::TCPServer( const String & address, uint16_t port ) :
		Socket( Socket::TCP_SOCKET )
	{
		// bind the address and port
		bind( address, port );
	}

	TCPClient* TCPServer::accept()
	{
		struct sockaddr otherHost;
		socklen_t addrSize = sizeof( otherHost );
		int fd = ::accept( _sockfd, &otherHost, &addrSize );

		if( fd < 0 ){
			String msg( "Accept: " );
			msg += strerror( errno );
			std::cout << msg << std::endl;
			return 0;
		} else {
			// got a new connection
			TCPClient * ret = new TCPClient( fd );
			return ret;
		}
	}
}
