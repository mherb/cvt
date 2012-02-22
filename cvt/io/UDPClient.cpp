/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/io/UDPClient.h>

namespace cvt
{
	UDPClient::UDPClient() : Socket( Socket::UDP_SOCKET )
	{
		bind( "0.0.0.0", 0 );
	}

	UDPClient::UDPClient( const String & addr, uint16_t port ) : Socket( Socket::UDP_SOCKET )
	{
		bind( addr, port );
	}
}
