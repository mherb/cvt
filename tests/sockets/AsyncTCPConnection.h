#ifndef ASYNC_TCP_CONNECTION_H
#define ASYNC_TCP_CONNECTION_H

#include "TCPServer.h"

#include <cvt/util/Signal.h>
#include <cvt/io/IOHandler.h>

namespace cvt
{
	class AsyncTCPConnection : public IOHandler
	{
		public:
			AsyncTCPConnection( TCPClient* socket );
			AsyncTCPConnection( const String & address, uint16_t port );
			~AsyncTCPConnection();

			void onDataReadable();
			void onDataWriteable();
			void onException(){}

			size_t send( const uint8_t* buf, size_t maxLen );
			size_t receive( uint8_t* buf, size_t maxLen );
			
			Signal<void>	canSend;
			Signal<void>	canReceive;

		private:
			TCPClient*		_socket;
	};

	inline AsyncTCPConnection::AsyncTCPConnection( TCPClient* socket ) : IOHandler( socket->socketDescriptor() )
		,_socket( socket )
	{
		notifyReadable( true );
		notifyWriteable( true );
	}

	inline AsyncTCPConnection::AsyncTCPConnection( const String & address, uint16_t port ) : IOHandler( -1 )
		,_socket( new TCPClient() )
	{
		_socket->connect( address, port );
		_fd = _socket->socketDescriptor();
		notifyReadable( true );
		notifyWriteable( true );
	}

	inline AsyncTCPConnection::~AsyncTCPConnection()
	{
		if( _socket )
			delete _socket;
	}


	inline void AsyncTCPConnection::onDataReadable()
	{
		canReceive.notify();
	}

	inline void AsyncTCPConnection::onDataWriteable()
	{
		canSend.notify();
	}


	inline size_t AsyncTCPConnection::send( const uint8_t* buf, size_t maxLen )
	{
		return _socket->send( buf, maxLen );
	}

	inline size_t AsyncTCPConnection::receive( uint8_t* buf, size_t bufSize )
	{
		return _socket->receive( buf, bufSize );
	}
}

#endif
