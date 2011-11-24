#ifndef CVT_TCPSERVER_H
#define CVT_TCPSERVER_H

#include "Socket.h"
#include "TCPClient.h"

namespace cvt
{
	class TCPServer : public Socket
	{
		public:
			TCPServer( const String & address, uint16_t port );

			/* accept incoming connection requests */
			TCPClient* accept();

			using Socket::listen;
			using Socket::send;
			using Socket::receive;
	};
}

#endif
