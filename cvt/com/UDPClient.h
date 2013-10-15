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
