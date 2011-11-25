#include <cvt/io/Host.h>
#include <cvt/io/AsyncTCPClient.h>
#include <cvt/io/AsyncTCPServer.h>
#include <cvt/io/AsyncUDPClient.h>
#include <cvt/io/TCPServer.h>
#include <cvt/io/TCPClient.h>
#include <cvt/io/UDPClient.h>

#include <cvt/util/Data.h>
#include <cvt/util/String.h>

#include <string>

// for addrinfo

using namespace cvt;

class ServerHandling
{
	public:
		ServerHandling( const String & serverAddr, uint16_t port ):
			_server( serverAddr, port ),
			_connections( 0 ),
			_buffer( 1024 )
		{
			_ios.registerIOHandler( &_server );

			Delegate<void ()> acceptDel( this, &ServerHandling::newConnection );
			_server.canAccept.add( acceptDel );
		}

		void newConnection()
		{
			if( _connections != 0 )
				throw CVTException( "Already connected" );

			_connections = new AsyncTCPClient( _server.accept() );

			Delegate<void ()> readDel( this, &ServerHandling::socketCanRead );
			_connections->canReceive.add( readDel );

			_ios.registerIOHandler( _connections );
		}

		void socketCanRead()
		{
			_numInBuffer = _connections->receive( _buffer.ptr(), _buffer.size() );

			String msg( ( const char* )_buffer.ptr(), _numInBuffer );
			std::cout << "Received: " << msg << std::endl;
		}

		void handleIO( int timeout )
		{
			_ios.handleIO( timeout );
		}

	private:
		AsyncTCPServer			_server;
		IOSelect				_ios;
		AsyncTCPClient*		_connections;
		Data					_buffer;
		size_t					_numInBuffer;
};

void asyncTcpServer()
{
	ServerHandling handler( "0.0.0.0", 12346 );
	while( true ){
		handler.handleIO( 1000 );
	}
}

class TCPClientHandler
{
	public:
		TCPClientHandler( const String & server, uint16_t port ) :
			_client( server, port )
		{
			_ios.registerIOHandler( &_client );

			Delegate<void ()> sendDel( this, &TCPClientHandler::socketCanSend );
			_client.canSend.add( sendDel );
		}

		void socketCanSend()
		{
			String message( "GOTTA MESSAGE FOR YOUU" );
			size_t n = _client.send( (const uint8_t*)message.c_str(), message.length() );
			if( n != message.length() ){
				std::cout << "Could not send whole message" << std::endl;
			}
		}

		void handleIO( int timeout )
		{
			_ios.handleIO( timeout );
		}

	private:
		AsyncTCPClient	_client;
		IOSelect		_ios;
};

void asyncTCPClient()
{
	TCPClientHandler tcpConn( "0.0.0.0", 12346 );
	
	while( true ){
		tcpConn.handleIO( 1000 );
		sleep( 1 );
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


class UDPHandler
{
	public:
		UDPHandler( AsyncUDPClient* client ) :
			_client( client ),
			_host( "0.0.0.0", 12345 ),
			_sendData( false )
		{
			_ios.registerIOHandler( _client );

			Delegate<void ()> sendDel( this, &UDPHandler::socketCanSend );
			Delegate<void ()> rcvDel( this, &UDPHandler::socketCanReceive );
			_client->canSend.add( sendDel );
			_client->canReceive.add( rcvDel );
		}

		void socketCanSend()
		{
			if( _sendData ){
				String message( "GOTTA MESSAGE FOR YOUU" );
				size_t n = _client->sendTo( _host, (const uint8_t*)message.c_str(), message.length() );
				if( n != message.length() ){
					std::cout << "Could not send whole message" << std::endl;
				}
			}
		}

		void socketCanReceive()
		{
			Data inData( 1024 );

			size_t n = _client->receiveFrom( _host, inData.ptr(), inData.size() );
			String msg( (const char*)inData.ptr(), n );
			std::cout << "Received:" << msg << std::endl;
		}

		void setDestination( const Host & host )
		{
			_host = host;
			_sendData = true;
		}

		void handleIO( int timeout )
		{
			_ios.handleIO( timeout );
		}

	private:
		AsyncUDPClient*	_client;
		IOSelect		_ios;
		Host			_host;

		bool			_sendData;
};

void asyncUDPServer()
{
	AsyncUDPClient client( "0.0.0.0", 12345 );
	UDPHandler handler( &client );

	while( true ){
		handler.handleIO( 1000 );
	}
}

void asyncUDPClient()
{
	AsyncUDPClient client;
	UDPHandler handler( &client );

	Host dest( "0.0.0.0", 12345 );
	handler.setDestination( dest );

	while( true ){
		handler.handleIO( 1000 );
		sleep( 1 );
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
	//	udpServer();
	//	asyncTcpServer();
		asyncUDPServer();
	} else {
	//	tcpClient();
	//	udpClient();
	//	asyncTCPClient();
		asyncUDPClient();
	}

	return 0;
}
