#include <cvt/math/Polynomial.h>
#include <cvt/util/CVTTest.h>

#include <iostream>

using namespace cvt;

BEGIN_CVTTEST( Polynomial )
	Polynomialf f( 1 );
	f[ 0 ] = 1.0f;
	f[ 1 ] = 1.0f;

	std::cout << f.eval( 3.0f ) << std::endl;
	return true;
END_CVTTEST
