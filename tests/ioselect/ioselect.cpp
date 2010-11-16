#include <cvt/util/Time.h>

#include <cvt/io/IOSelect.h>
#include <cvt/io/IOHandler.h>

#include <iostream>

using namespace cvt;

class MyIOHandler : public IOHandler
{
	public:
	MyIOHandler() : IOHandler( 0 )
	{
	}

	void onDataReadable()
	{
		ssize_t len;
		char buf[ 101 ];
		std::cout << "Data available" << std::endl;
		len = read( 0, buf, 100 );
		buf[ len - 1 ] = '\0';
		std::cout << "GOT: " << buf << std::endl;
	}

	void onDataWriteable()
	{
	}

	void onException()
	{
	}
};

int main()
{
	MyIOHandler myio;
	myio.notifyReadable( true );
	IOSelect ios;

	ios.registerIOHandler( &myio );

	while( 1 ) {
		int n;
		n = ios.handleIO( 1000 );
		if( !n )
			std::cout << "timeout" << std::endl;
	}

}
