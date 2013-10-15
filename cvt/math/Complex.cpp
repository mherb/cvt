#include <cvt/math/Complex.h>
#include <cvt/util/CVTTest.h>

using namespace cvt;

BEGIN_CVTTEST( Complex )
	bool b = true;
	Complexf c1( 1.0f, 2.0f );
	Complexf c2( 3.0f, 4.0f );

	b &= ( c1 + c2 ) == Complexf( 4.0f, 6.0f );
	CVTTEST_PRINT( "+", b );

	b &= ( c1 - c2 ) == Complexf( -2.0f, -2.0f );
	CVTTEST_PRINT( "-", b );

	b &= ( c1 * c2 ) == Complexf( -5.0f, 10.0f );
	CVTTEST_PRINT( "*", b );

	b &= ( c1 / c2 ) == Complexf( 11.0f / 25.0f, 2.0f / 25.0f );
	CVTTEST_PRINT( "/", b );

	Complexf ctmp = c1;
	b &= ( ctmp += c2 ) == Complexf( 4.0f, 6.0f );
	CVTTEST_PRINT( "+=", b );

	ctmp = c1;
	b &= ( ctmp -= c2 ) == Complexf( -2.0f, -2.0f );
	CVTTEST_PRINT( "-=", b );

	ctmp = c1;
	b &= ( ctmp *= c2 ) == Complexf( -5.0f, 10.0f );
	CVTTEST_PRINT( "*=", b );

	ctmp = c1;
	b &= ( ctmp /= c2 ) == Complexf( 11.0f / 25.0f, 2.0f / 25.0f );
	CVTTEST_PRINT( "/=", b );


	b &= ( c1 + 3.0f ) == Complexf( 4.0f, 2.0f );
	CVTTEST_PRINT( "+", b );

	b &= ( c1 - 3.0f ) == Complexf( -2.0f, 2.0f );
	CVTTEST_PRINT( "-", b );

	b &= ( c1 * 3.0f ) == Complexf( 3.0f, 6.0f );
	CVTTEST_PRINT( "*", b );

	b &= ( c1 / 3.0f ) == Complexf( 1.0f / 3.0f, 2.0f / 3.0f );
	CVTTEST_PRINT( "/", b );

	ctmp = c1;
	b &= ( ctmp += 3.0f ) == Complexf( 4.0f, 2.0f );
	CVTTEST_PRINT( "+=", b );

	ctmp = c1;
	b &= ( ctmp -= 3.0f ) == Complexf( -2.0f, 2.0f );
	CVTTEST_PRINT( "-=", b );

	ctmp = c1;
	b &= ( ctmp *= 3.0f ) == Complexf( 3.0f, 6.0f );
	CVTTEST_PRINT( "*=", b );

	ctmp = c1;
	b &= ( ctmp /= 3.0f ) == Complexf( 1.0f / 3.0f, 2.0f / 3.0f );
	CVTTEST_PRINT( "/=", b );

	b &= c1.conj() == Complexf( c1.re, - c1.im );
	CVTTEST_PRINT( "conj()", b );

	return b;

END_CVTTEST
