#ifndef ASYNC_TCP_CONNECTION_H
#define ASYNC_TCP_CONNECTION_H

#include "Socket.h"
#include <sys/ioctl.h>

#include <cvt/util/Signal.h>
#include <cvt/util/Data.h>
#include <cvt/io/IOHandler.h>

namespace cvt
{
	class AsyncTCPConnection : public IOHandler
	{
		public:
			AsyncTCPConnection( Socket* socket );
			~AsyncTCPConnection();

			void onDataReadable();
			void onDataWriteable();
			void onException(){}

			/* only notify observers when at leas s bytes have been read */
			void setMinReadSize( size_t s ) { _minNotificationSize = s; }

	//		void setWriteBufferSize( size_t s );
	//		size_t write( const uint8_t* buf, size_t maxLen );
			
			/* signal that new data has been read  */
			Signal<const Data&>	dataReceived;

		private:
			Socket*	_socket;

			Data			_dataIn;
			uint8_t*		_inPtr;

			size_t			_minNotificationSize;
	};

	inline AsyncTCPConnection::AsyncTCPConnection( Socket* socket ) : IOHandler( socket->socketDescriptor() )
		,_socket( socket )
		,_dataIn( 4096 )
		,_inPtr( _dataIn.ptr() )
		,_minNotificationSize( 1 )
	{
	}

	inline AsyncTCPConnection::~AsyncTCPConnection()
	{
		delete _socket;
	}


	inline void AsyncTCPConnection::onDataReadable()
	{
		int n;
		ioctl( _fd, FIONREAD, &n );
		std::cout << "DATA AVAILABLE: " << n << std::endl;

		size_t alreadyRead = _dataIn.ptr() - _inPtr;
		size_t remainingInBytes = _dataIn.size() - alreadyRead;
		size_t bytesRead = _socket->receive( _inPtr, remainingInBytes );
		size_t all = bytesRead + alreadyRead;
		if( all < _minNotificationSize ){
			_inPtr += bytesRead;
		} else {
			// notify about data
			// create Data referencing received data
			Data received( _dataIn.ptr(), all, false );
			dataReceived.notify( received );

			// invalidate the dataIter
			_inPtr = _dataIn.ptr();
		}
	}

	inline void AsyncTCPConnection::onDataWriteable()
	{
		std::cout << "Socket can Write" << std::endl;
	}
}

#endif
