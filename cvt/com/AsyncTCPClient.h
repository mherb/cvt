#ifndef CVT_ASYNC_TCP_CLIENT_H
#define CVT_ASYNC_TCP_CLIENT_H

#include <cvt/com/TCPServer.h>
#include <cvt/com/TCPClient.h>

#include <cvt/util/Signal.h>
#include <cvt/io/IOHandler.h>

namespace cvt
{
	class AsyncTCPClient : public IOHandler
	{
		public:
			AsyncTCPClient( TCPClient* socket );
			AsyncTCPClient( const String & address, uint16_t port );
			~AsyncTCPClient();

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

	inline AsyncTCPClient::AsyncTCPClient( TCPClient* socket ) : IOHandler( socket->socketDescriptor() )
		,_socket( socket )
	{
		notifyReadable( true );
		notifyWriteable( true );
	}

	inline AsyncTCPClient::AsyncTCPClient( const String & address, uint16_t port ) : IOHandler( -1 )
		,_socket( new TCPClient() )
	{
		_socket->connect( address, port );
		_fd = _socket->socketDescriptor();
		notifyReadable( true );
		notifyWriteable( true );
	}

	inline AsyncTCPClient::~AsyncTCPClient()
	{
		if( _socket )
			delete _socket;
	}


	inline void AsyncTCPClient::onDataReadable()
	{
		canReceive.notify();
	}

	inline void AsyncTCPClient::onDataWriteable()
	{
		canSend.notify();
	}


	inline size_t AsyncTCPClient::send( const uint8_t* buf, size_t maxLen )
	{
		return _socket->send( buf, maxLen );
	}

	inline size_t AsyncTCPClient::receive( uint8_t* buf, size_t bufSize )
	{
		return _socket->receive( buf, bufSize );
	}
}

#endif
