/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_TCPSOCKET_H
#define CVT_TCPSOCKET_H

#include <cvt/io/Socket.h>

namespace cvt
{
	class TCPClient : public Socket
	{
		friend class TCPServer;
		public:
			TCPClient();
			using Socket::connect;
			using Socket::send;
			using Socket::receive;

		private:
			TCPClient( int fd );
	};
}

#endif
