#include <cvt/util/Signal.h>

#include <iostream>

using namespace cvt;

static void func( int x )
{
	std::cout << "BLA " << x << std::endl;
}

static void funcX( int x )
{
	std::cout << "BLUB " << x << std::endl;
}

static void funcY()
{
	std::cout << "i am void " << std::endl;
}

static void funcYfalse()
{
	std::cout << "i am void " << std::endl;
}

class BLA {
	public:
		void blub( ) { std::cout << "BLA::blub" << std::endl; }
};

int main()
{
	Signal<int> a;
	Delegate<void (int)> d( &func );
	Delegate<void (int)> d2( &funcX );

	Signal<void> b;
	Delegate<void ()> xx( &funcY );
	b.add( xx );
	b.notify();

	Delegate<void ()> xx2( xx );
	Delegate<void ()> xx3( &funcYfalse );
	if( xx == xx2 )
		std::cout << "CORRECT" << std::endl;
	if( xx != xx3 )
		std::cout << "CORRECT" << std::endl;

	BLA bla;

	Delegate<void ()> bb( &bla, &BLA::blub );
	Delegate<void ()> bb2( &bla, &BLA::blub );
	Delegate<void ()> bb3( bb );
	bb();
	if( bb != xx2 )
		std::cout << "CORRECT" << std::endl;
	if( bb == bb2 )
		std::cout << "CORRECT" << std::endl;
	if( bb2 == bb3 )
		std::cout << "CORRECT" << std::endl;

	a.add( d );
	a.add( d2 );
	a.notify( 10 );

}
