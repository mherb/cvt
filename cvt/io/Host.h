/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_HOST_H
#define CVT_HOST_H

#include <cvt/util/String.h>
#include <netdb.h>

namespace cvt
{
	enum ProtocolFamily
	{
		IPV4 = PF_INET,
		IPV6 = PF_INET6,
		IPV_UNSPEC = PF_UNSPEC
	};

	class Host
	{
		public:
			friend class Socket;
			Host( const String & address, const String & service, ProtocolFamily protFam = IPV_UNSPEC );
			Host( const String & address, uint16_t port, ProtocolFamily protFam = IPV_UNSPEC );
			Host();
			~Host();

			void lookup( const String & address, const String & service, ProtocolFamily protocolFamily );
			uint16_t port() const;

			String addressAsString();

		private:
			struct sockaddr_storage	_sockAddress;
			socklen_t				_addressLen;

	};
}

#endif
