#include <cvt/util/Time.h>
#include <iostream>

using namespace cvt;


int main()
{
	Time t, t2;
	t.reset();
	usleep( 500 );
	t2.reset();
	std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;
	std::cout << t.elapsedMicroSeconds() << " us" << std::endl;
	std::cout << t2 - t << " ms" << std::endl;
	std::cout << t2 - t.ms() << " ms" << std::endl;

}
