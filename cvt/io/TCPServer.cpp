/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/io/TCPServer.h>

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
