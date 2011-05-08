#include <cvt/math/VectorN.h>

using namespace cvt;

int main()
{
	Vector<float> a( 10 );
	Vector<float> b( 10 );
	Vector<float> c( 10 );

	a.fill( 0 );
	b.fill( 1 );
	c.fill( 2 );

	a = 5.0f *  b * 2.0f + 2.0f * c;


	for( size_t i = 0; i < a.size(); i++ )
		std::cout << a[ i ] << std::endl;

}
