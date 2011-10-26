#include <iostream>
#include <cvt/math/BSpline.h>

using namespace cvt;

int main()
{
	for( float t = -2.5f; t <= 2.5f; t += 0.01f )
		std::cout << t
				  << " " << BSplinef::eval( t )
				  << " " << BSplinef::evalDerivative( t )
				  << " " << BSplinef::evalSecondDerivative( t )
				  << " " << -BSplinef::evalDerivative( t ) / BSplinef::evalSecondDerivative( t )
				  << std::endl;
	return 0;
}
