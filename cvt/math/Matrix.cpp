#include <cvt/math/Matrix.h>
#include <cvt/util/CVTTest.h>

#include <iostream>

namespace cvt {

	template<>
	bool Matrix2<float>::inverseSelf()
	{
		float D, invD, tmp;

		D = mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1];

		if ( Math::abs( D ) < Math::EPSILONF )
			return false;

		invD = 1.0f / D;

		tmp = mat[0][0];
		mat[0][0] =   mat[1][1] * invD;
		mat[0][1] = - mat[0][1] * invD;
		mat[1][0] = - mat[1][0] * invD;
		mat[1][1] =   tmp * invD;

		return true;
	}

	template<>
	bool Matrix2<double>::inverseSelf()
	{
		double D, invD, tmp;

		D = mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1];

		if ( Math::abs( D ) < Math::EPSILOND )
			return false;

		invD = 1.0f / D;

		tmp = mat[0][0];
		mat[0][0] =   mat[1][1] * invD;
		mat[0][1] = - mat[0][1] * invD;
		mat[1][0] = - mat[1][0] * invD;
		mat[1][1] =   tmp * invD;

		return true;
	}

	BEGIN_CVTTEST(Matrix2)
		Matrix2f mat;
		std::cout << mat << std::endl;
		mat.identity();
		std::cout << mat << std::endl;
		mat *= 2.0f;
		std::cout << mat << std::endl;
		mat.inverseSelf();
		std::cout << mat << std::endl;
		return true;
	END_CVTTEST

}
