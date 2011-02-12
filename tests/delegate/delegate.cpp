#include <cvt/util/Delegate.h>
#include <cvt/util/Time.h>

#include <iostream>


using namespace cvt;

class A {
	public:
		virtual ~A(){} 
		virtual int bla(  ) { /*std::cout << "A::bla() " << std::endl;*/ return 0; }
		int bla( int x ) { std::cout << "A::bla( int ) " << x << std::endl; return 0; }
		int blo( int x ) const { std::cout << "A::blo " << x << std::endl; return 0; }
		void blim( int x1, int , int, int, int, int, int, int, int, int x10 ) const { std::cout << "A::blim " << x1 << " " << x10 << std::endl; };
};

int blub( int x )
{
	std::cout << "blub " << x << std::endl;
	return 0;
}

static int bli( int x )
{
	std::cout << "bli " << x << std::endl;
	return 0;
}

void bla( )
{
	std::cout << "bla " << std::endl;
}


int main()
{
	A a;
	Delegate<int (int)> d1( &a, &A::bla );
	Delegate<int (int)> d2( &bli );
	Delegate<int (int)> d3( &blub );
	Delegate<int (int)> d4( &a, &A::blo );
	Delegate<void ()> d5( &bla );
	Delegate<void (int, int, int, int, int, int, int, int, int, int )> d6( &a, &A::blim );
	Delegate<int ()> d7( &a, &A::bla );

	d1( 10 );
	d2( 20 );
	d3( 30 );
	d4( 40 );
	d5( );
	d6( 1, 2, 3, 4, 5, 6, 7, 8 ,9, 10 );

#define CALLMAX 1000000

	Time t;
	t.reset();
	for( size_t i = 0; i < CALLMAX; i++ )
		a.bla();
	std::cout << "Time: " << t.elapsedMilliSeconds() << std::endl;

	t.reset();
	for( size_t i = 0; i < CALLMAX; i++ )
		d7();
	std::cout << "Time: " << t.elapsedMilliSeconds() << std::endl;

}
