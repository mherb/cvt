#include <cvt/util/Delegate.h>
#include <iostream>


using namespace cvt;

class A {
	public:
		int bla( int x ) { std::cout << "A::bla " << x << std::endl; };
		int blo( int x ) const { std::cout << "A::blo " << x << std::endl; };
};

int blub( int x )
{
	std::cout << "blub " << x << std::endl;
}

static int bli( int x )
{
	std::cout << "bli " << x << std::endl;
}

int main()
{
	A a;
	Delegate<int (int)> d1( &a, &A::bla );
	Delegate<int (int)> d2( &bli );
	Delegate<int (int)> d3( &blub );
	Delegate<int (int)> d4( &a, &A::blo );

	d1( 10 );
	d2( 20 );
	d3( 30 );
	d4( 40 );
}
