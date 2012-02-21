/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_TCPSERVER_H
#define CVT_TCPSERVER_H

#include <cvt/io/Socket.h>
#include <cvt/io/TCPClient.h>

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
