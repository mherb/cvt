#include <cvt/math/Polynomial.h>
#include <cvt/util/CVTTest.h>


using namespace cvt;

BEGIN_CVTTEST( Polynomial )
	bool b = true;
	Polynomialf p1( 2.0f, 1.0f );
	Polynomialf p2( 3.0f, 2.0f, 1.0f );

	Polynomialf tmp = p1 + p1;
	b &= tmp == 2.0f * p1;
	CVTTEST_PRINT( "+", b );
	CVTTEST_PRINT( "*", b );

	tmp = -1.0f * p1;
	b &= tmp == -p1;
	CVTTEST_PRINT( "-", b );

	tmp = p1 - p1;
	b &= tmp.degree() == 0;
	CVTTEST_PRINT( "-", b );

	Polynomialf tmp2( 2 );
	tmp2.setZero();
	b &= tmp == tmp2;
	CVTTEST_PRINT( "setZero()", b );

	Complexf roots[ 4 ];
	Polynomialf poly( 1.0f, -10.0f, 35.0f, -50.0f, 24.0f );
	poly.roots( roots );
	for( size_t i = 0; i < 4; i++ )
		std::cout << roots[ i ] << std::endl;

{
	Complexd roots[ 4 ];
	Polynomiald poly( 1.0, -10.0, 35.0, -50.0, 24.0 );
	poly.roots( roots );
	for( size_t i = 0; i < 4; i++ )
		std::cout << roots[ i ] << std::endl;
}

	return b;
END_CVTTEST
