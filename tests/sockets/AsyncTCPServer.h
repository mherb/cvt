#ifndef CVT_ASYNC_TCP_SERVER_H
#define CVT_ASYNC_TCP_SERVER_H

#include "Socket.h"
#include <cvt/io/IOHandler.h>
#include <cvt/util/Signal.h>
#include <cvt/util/String.h>

namespace cvt
{
	class AsyncTCPServer : public IOHandler  
	{
		public:
			AsyncTCPServer( const String & address, uint16_t port, int maxConnections = 10 );

			/* the server is only accepting new connections */
			void onDataReadable();

			void onDataWriteable()
			{
				std::cout << "Data Writeable" << std::endl;
			}

			void onException(){}

			Signal<AsyncTCPConnection*>	newConnection;

		private:
			Socket	_socket;
			int		_maxConnections;
	};

	inline AsyncTCPServer::AsyncTCPServer( const String & address, uint16_t port, int maxConnections ) : IOHandler( -1 )
		,_socket( Socket::TCP_SOCKET )
		,_maxConnections( maxConnections )
	{
		// bind to the port
		_socket.bind( address, port );

		_fd = _socket.socketDescriptor();
		notifyReadable( true );

		_socket.listen( _maxConnections );	
	}

	inline void AsyncTCPServer::onDataReadable()
	{
		// this should be an accept:
		Socket* newSock = _socket.accept();
		std::cout << "Accepted connection" << std::endl;
		if( newSock ){
			newConnection.notify( new AsyncTCPConnection( newSock ) );
		}		
	}
}

#endif
