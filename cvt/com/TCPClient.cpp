/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/com/TCPClient.h>

namespace cvt
{
	TCPClient::TCPClient() : Socket( Socket::TCP_SOCKET )
	{
	}

	TCPClient::TCPClient( int fd ) :
		Socket( Socket::TCP_SOCKET, fd )
	{
	}
}
