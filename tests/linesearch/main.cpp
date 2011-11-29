#include <cvt/math/Math.h>

using namespace cvt;

float func( float x )
{
	return -( x - 1.0f ) * ( x - 1.0f );
}

struct Bla {
float operator()( float x )
{
	return -( x - 1.0f ) * ( x - 1.0f );
}
};

int main()
{
	Bla b;
	float x = Math::lineSearchMaxGolden( -10.0f, 10.0f, b );
	std::cout << x << std::endl;
}
