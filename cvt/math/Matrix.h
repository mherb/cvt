#ifndef CVT_MATRIX_H
#define CVT_MATRIX_H

#include <iostream>
#include <iomanip>

#include <cvt/math/Math.h>

#include <cvt/math/Vector.h>

#include <cvt/math/Matrix2.h>
#include <cvt/math/Matrix3.h>
#include <cvt/math/Matrix4.h>

namespace cvt {
	/**
	  \ingroup Math
	*/
	typedef Matrix2<float> Matrix2f;
	typedef Matrix2<double> Matrix2d;

	typedef Matrix3<float> Matrix3f;
	typedef Matrix3<double> Matrix3d;

	typedef Matrix4<float> Matrix4f;
	typedef Matrix4<double> Matrix4d;

	template<int dim, typename T>
		struct Matrix {
			typedef T TYPE;
		};

	template<typename T>
		struct Matrix<2, T> {
			typedef Matrix2<T> TYPE;
		};

	template<typename T>
		struct Matrix<3, T> {
			typedef Matrix3<T> TYPE;
		};

	template<typename T>
		struct Matrix<4, T> {
			typedef Matrix4<T> TYPE;
		};
}

#endif
