#include "Host.h"
#include "AsyncTCPConnection.h"
#include "AsyncTCPServer.h"
#include <cvt/util/Data.h>
#include <cvt/util/String.h>
#include <cvt/io/IOSelect.h>

#include "TCPServer.h"
#include "TCPClient.h"
#include "UDPClient.h"

#include <string>

// for addrinfo

using namespace cvt;

class ConnectionHandler
{
	public:
		ConnectionHandler()
		{
		}
		
		~ConnectionHandler()
		{
			for( size_t i = 0; i < _connections.size(); i++ ){
				delete _connections[ i ];
			}
		}

		int handleIO( int to )
		{
			return _ios.handleIO( to );
		}

		void newConnection( AsyncTCPConnection* conn )
		{
			std::cout << "Got New Connection" << std::endl;
			_connections.push_back( conn );
			conn->notifyReadable( true );
			//conn->notifyWriteable( true );

			Delegate<void (const Data&)> d( this, &ConnectionHandler::dataReceived );
			conn->dataReceived.add( d );

			registerIOHandler( conn );
		}

		void registerIOHandler( IOHandler* handler )
		{
			_ios.registerIOHandler( handler );
		}

		void dataReceived( const Data& d )
		{
			String str( (const char*)d.ptr(), d.size() );
			std::cout << "Received: " << str << std::endl;
		}

	private:
		std::vector<AsyncTCPConnection*>	_connections;
		IOSelect							_ios;
};

void asyncTcpServer()
{
	ConnectionHandler connHandler;
	AsyncTCPServer server( "0.0.0.0", 12346 );
	connHandler.registerIOHandler( &server );

	Delegate<void ( AsyncTCPConnection* )> d( &connHandler, &ConnectionHandler::newConnection );
	server.newConnection.add( d );

	while( true ){
		connHandler.handleIO( 1000 );
	}
}

void asyncTCPClient()
{
	IOSelect	iohandler;	
	AsyncTCPConnection tcpConn( "0.0.0.0", 12346 );
	tcpConn.notifyWriteable( true );
	
	iohandler.registerIOHandler( &tcpConn );

	while( true ){
		std::string line;
		std::getline( std::cin, line );

		String buf( line.c_str() );
		size_t toSend = buf.length();
		const uint8_t * data = (const uint8_t*)buf.c_str();
		while( toSend > 0 ){	
			size_t sent = tcpConn.send( data, toSend );
			data += sent;
			toSend -= sent;
			iohandler.handleIO( 10 );
		}
	}
}

void tcpClient()
{
	TCPClient client;
	client.connect( "0.0.0.0", 12346 );

	// client connected
	std::cout << "Client connected to Server" << std::endl;

	while( true ){
		std::string line;
		std::getline( std::cin, line );

		std::cout << "Sending string: " << line << std::endl;

		String buf( line.c_str() );
		size_t toSend = buf.length();
		const uint8_t * data = (const uint8_t*)buf.c_str();
		while( toSend > 0 ){	
			size_t sent = client.send( data, toSend );
			data += sent;
			toSend -= sent;
		}
	}
}

void tcpServer()
{
	TCPServer server( "0.0.0.0", 12346 );
	server.listen();

	std::cout << "Listening" << std::endl;

	TCPClient * commSock = 0;
	while( commSock == 0 ){
		std::cout << "Awaiting accept success ... ";
		commSock = server.accept();
		std::cout << "Accepted" << std::endl;
	}

	const size_t bufSize = 1024;
	uint8_t buffer[ bufSize ];

	while( true) {
		// receive data
		memset( buffer, 0, bufSize );
		size_t num = commSock->receive( buffer, bufSize );

		if( num > 0 ){
			std::cout << buffer << std::endl;
		}
	}
}

void udpClient()
{
	UDPClient client;
	
	Host dest( "0.0.0.0", "12345" );

	String message( "Hello Server" );
	while( true ){
		size_t toSend = message.length();
		const uint8_t* data = ( const uint8_t* )message.c_str();
		while( toSend > 0 ){	
			size_t sent = client.sendTo( dest, data, toSend );
			data += sent;
			toSend -= sent;
		}
	}
}

void udpServer()
{
	UDPClient server( "0.0.0.0", 12345 );

	Host from;

	const size_t bufSize = 1024;
	uint8_t buffer[ bufSize ];

	while( true) {
		// receive data
		memset( buffer, 0, bufSize );
		size_t num = server.receiveFrom( from, buffer, bufSize );

		if( num > 0 ){
			std::cout << "Received from " << from.addressAsString() << ": ";
			std::cout << buffer << std::endl;
		}
	}
}

int main( int argc, char* argv[] )
{
	if( argc < 2 ){
		std::cout << "Usage: " << argv[ 0 ] << " <0=SERVER | 1=CLIENT>" << std::endl;
		return 0;
	}

	int type = atoi( argv[ 1 ] );
	if( type != 0 && type != 1 ){
		std::cout << "Usage: " << argv[ 0 ] << " <0=SERVER | 1=CLIENT>" << std::endl;
		return 0;
	}

	if( type == 0 ){
	//	tcpServer();
		udpServer();
	//	asyncTcpServer();
	} else {
	//	tcpClient();
	//	asyncTCPClient();
		udpClient();
	}

	return 0;
}
