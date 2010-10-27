#include <cvt/util/Thread.h>
#include <iostream>


using namespace cvt;

class MyThread : public Thread<int>
{
	public:
		MyThread() {};
		void execute( int* val );
};

void MyThread::execute( int* val )
{
	std::cout << "THREAD start" << std::endl;
	std::cout << "THREAD value: " << *val << std::endl;
	sleep( *val );
	std::cout << "THREAD end" << std::endl;
}

int main()
{
	MyThread t;
	int val = 2;

	t.run( &val );

	t.join();
	std::cout << "EXIT" << std::endl;
}
