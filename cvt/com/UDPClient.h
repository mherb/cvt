/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_UDPCLIENT_H
#define CVT_UDPCLIENT_H

#include <cvt/com/Socket.h>

namespace cvt
{
	class UDPClient : public Socket
	{
		public:
			/* create UDP Socket at arbritrary port */
			UDPClient();
			
			/* create UDP Socket with address & port */
			UDPClient( const String & address, uint16_t port );

			using Socket::sendTo;
			using Socket::receiveFrom;
			using Socket::connect;
	};
}

#endif
