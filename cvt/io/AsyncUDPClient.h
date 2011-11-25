#ifndef CVT_ASYNC_UDPCLIENT_H
#define CVT_ASYNC_UDPCLIENT_H

#include <cvt/io/UDPClient.h>
#include <cvt/util/Signal.h>
#include <cvt/io/IOHandler.h>

namespace cvt
{
	class AsyncUDPClient : public IOHandler
	{
		public:
			AsyncUDPClient();
			AsyncUDPClient( const String & address, uint16_t port );
			~AsyncUDPClient();

			void onDataReadable();
			void onDataWriteable();
			void onException(){}

			/* only notify observers when at leas s bytes have been read */
			size_t sendTo( const Host& receiver, const uint8_t* buf, size_t maxLen );
			size_t receiveFrom( Host& sender, uint8_t* buf, size_t bufSize );
			
			Signal<void>	canSend;
			Signal<void>	canReceive;

		private:
			UDPClient*		_socket;
	};

	inline AsyncUDPClient::AsyncUDPClient() : IOHandler( -1 )
		,_socket( new UDPClient() )
	{
		_fd = _socket->socketDescriptor();
		notifyReadable( true );
		notifyWriteable( true );
	}

	inline AsyncUDPClient::AsyncUDPClient( const String & address, uint16_t port ) : IOHandler( -1 )
		,_socket( new UDPClient( address, port ) )
	{
		_fd = _socket->socketDescriptor();
		notifyReadable( true );
		notifyWriteable( true );
	}

	inline AsyncUDPClient::~AsyncUDPClient()
	{
		if( _socket )
			delete _socket;
	}


	inline void AsyncUDPClient::onDataReadable()
	{
		canReceive.notify();
	}	

	inline void AsyncUDPClient::onDataWriteable()
	{
		canSend.notify();
	}

	inline size_t AsyncUDPClient::sendTo( const Host & h, const uint8_t* buf, size_t maxLen )
	{
		return _socket->sendTo( h, buf, maxLen );		
	}

	inline size_t AsyncUDPClient::receiveFrom( Host & h, uint8_t* buf, size_t bufSize )
	{
		return _socket->receiveFrom( h, buf, bufSize );		
	}
}

#endif
