#include <cvt/math/Matrix.h>
#include <cvt/util/CVTTest.h>

#include <iostream>

namespace cvt {

	BEGIN_CVTTEST(Matrix2)
		Matrix2f mat;
		std::cout << mat << std::endl;
		mat.identity();
		std::cout << mat << std::endl;
		mat *= 2.0f;
		std::cout << mat << std::endl;
		return true;
	END_CVTTEST

}
