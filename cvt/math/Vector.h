#ifndef CVT_VECTOR_H
#define CVT_VECTOR_H

#include <iostream>
#include <iomanip>
#include <cvt/math/Math.h>
#include <cvt/util/String.h>

#include <cvt/math/Vector2.h>
#include <cvt/math/Vector3.h>
#include <cvt/math/Vector4.h>

namespace cvt {
	typedef Vector2<float> Vector2f;
	typedef Vector2<double> Vector2d;
	typedef Vector2<int32_t> Vector2i;

	typedef Vector3<float> Vector3f;
	typedef Vector3<double> Vector3d;

	typedef Vector4<float> Vector4f;
	typedef Vector4<double> Vector4d;

	typedef Vector2f Point2f;
	typedef Vector3f Point3f;

	template<int dim, typename T>
		struct Vector {
			typedef T TYPE;
		};

	template<typename T>
		struct Vector<2, T> {
			typedef Vector2<T> TYPE;
		};

	template<typename T>
		struct Vector<3, T> {
			typedef Vector3<T> TYPE;
		};

	template<typename T>
		struct Vector<4, T> {
			typedef Vector4<T> TYPE;
		};

}

#endif
