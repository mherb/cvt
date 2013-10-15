#ifndef CVT_TCPSOCKET_H
#define CVT_TCPSOCKET_H

#include <cvt/com/Socket.h>

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
