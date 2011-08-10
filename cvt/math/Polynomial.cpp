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

	return b;
END_CVTTEST
