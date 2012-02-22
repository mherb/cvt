/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_SOCKET_H
#define CVT_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <cvt/io/Host.h>
#include <cvt/util/String.h>


namespace cvt
{
	class Socket 
	{
		public:
			enum SocketType {
				TCP_SOCKET = SOCK_STREAM,
				UDP_SOCKET = SOCK_DGRAM
			};

			void close();
			
			/* get the socket file descriptor (for select) */
			int socketDescriptor() const { return _sockfd; };

			size_t bytesAvailableForReading();

		protected:
			Socket( SocketType type, int fd = -1 );
			virtual ~Socket();
			/* bind this socket to a certain address */
			void bind( const String & addr, uint16_t port );

			/* create connection to another socket */
			void connect( const String & addr, uint16_t port );

			/* listen for incoming connection requests */
			void listen( int backlog = 10 );

			/* send data to connected peer */
			size_t send( const uint8_t* data, size_t len );

			/* reveive data from connected peer */
			size_t receive( uint8_t* data, size_t maxSize );

			/* send data to a specific host (UDP) */
			size_t sendTo( const Host & host, const uint8_t* data, size_t len );

			/* reveive data (UDP), host tells the peer that sent the data  */
			size_t receiveFrom( Host & host, uint8_t* data, size_t maxLen );

			static void fillAdressInfo( struct addrinfo & info, 
									    const String & address, 
									    const String & service, 
									    SocketType type );

			int				_sockfd;
			SocketType		_sockType;

			void setReusable();
	};
}

#endif
