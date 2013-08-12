#include <cvt/math/VectorN.h>
#include <cvt/math/MatrixNM.h>
#include <cvt/math/MVExpr.h>

using namespace cvt;

int main()
{
	Matrix<3,3,float> a;
	Matrix<3,3,float> b;
	Matrix<3,3,float> c;

	a.setIdentity();
	b.setIdentity();

	std::cout << a << std::endl;
	std::cout << b << std::endl;

	a( 0, 0 ) = 2.0f;
	a( 1, 1 ) = 4.0f;
	a( 2, 2 ) = 8.0f;
	c = a * b;

	std::cout << c << std::endl;
}
