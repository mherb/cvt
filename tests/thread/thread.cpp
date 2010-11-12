#include <cvt/util/Thread.h>
#include <cvt/util/TQueue.h>
#include <iostream>


using namespace cvt;

class MyThread : public Thread<void>
{
	public:
		MyThread( TQueue<int>* q ) : _q( q ) {};
		void execute(void*);
	private:
		TQueue<int>* _q;
};

void MyThread::execute( void* )
{
	std::cout << "THREAD start" << std::endl;
	for( int i = 0; i < 10; i++) {
		_q->enqueue( i );
		sleep( 1 );
	}
	std::cout << "THREAD end" << std::endl;
}

int main()
{
	TQueue<int> q;
	MyThread t( &q );
	int val;

	t.run( NULL );

	do {
		val = q.waitNext();
		std::cout << "Value: " << val << std::endl;
	} while( val != 9 );

	t.join();
}
