#ifndef ASYNC_TCP_CONNECTION_H
#define ASYNC_TCP_CONNECTION_H

#include "TCPServer.h"
#include <sys/ioctl.h>

#include <cvt/util/Signal.h>
#include <cvt/util/Data.h>
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

			/* only notify observers when at leas s bytes have been read */
			void setMinReadSize( size_t s ) { _minNotificationSize = s; }

			size_t send( const uint8_t* buf, size_t maxLen );
			
			/* signal that new data has been read  */
			Signal<const Data&>	dataReceived;

		private:
			TCPClient*		_socket;

			Data			_dataIn;
			uint8_t*		_inPtr;
			Data			_dataOut;
			uint8_t*		_outPtr;
			uint8_t*		_outPtrSend;

			size_t			_minNotificationSize;
	};

	inline AsyncTCPConnection::AsyncTCPConnection( TCPClient* socket ) : IOHandler( socket->socketDescriptor() )
		,_socket( socket )
		,_dataIn( 4096 )
		,_inPtr( _dataIn.ptr() )
		,_dataOut( 4096 )
		,_outPtr( _dataOut.ptr() )
		,_outPtrSend( _dataOut.ptr() )
		,_minNotificationSize( 1 )
	{
	}

	inline AsyncTCPConnection::AsyncTCPConnection( const String & address, uint16_t port ) : IOHandler( -1 )
		,_socket( new TCPClient() )
		,_dataIn( 4096 )
		,_inPtr( _dataIn.ptr() )
		,_dataOut( 4096 )
		,_outPtr( _dataOut.ptr() )
		,_outPtrSend( _dataOut.ptr() )
		,_minNotificationSize( 1 )
	{
		_socket->connect( address, port );
		_fd = _socket->socketDescriptor();
	}

	inline AsyncTCPConnection::~AsyncTCPConnection()
	{
		if( _socket )
			delete _socket;
	}


	inline void AsyncTCPConnection::onDataReadable()
	{
		/*
		int n;
		ioctl( _fd, FIONREAD, &n );
		std::cout << "DATA AVAILABLE: " << n << std::endl;
		*/

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
		size_t toSend = _outPtr - _outPtrSend;
		size_t numSent = _socket->send( _outPtrSend, toSend );

		if( toSend == numSent ){
			_outPtr = _dataOut.ptr();
			_outPtrSend = _outPtr;
		} else {
			_outPtrSend += numSent;
		}
	}


	inline size_t AsyncTCPConnection::send( const uint8_t* buf, size_t maxLen )
	{
		size_t numInBuffer = _outPtr - _dataOut.ptr();
		size_t remaining   = _dataOut.size() - numInBuffer;
		size_t numWritten = Math::min( remaining, maxLen );
		
		memcpy( _outPtr, buf, numWritten );
		_outPtr += numWritten;

		return numWritten;		
	}
}

#endif
