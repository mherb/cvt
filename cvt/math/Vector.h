#ifndef CVT_VECTOR_H
#define CVT_VECTOR_H

#include <iostream>
#include <cvt/math/Math.h>

#include <cvt/math/Vector2.h>
#include <cvt/math/Vector3.h>
#include <cvt/math/Vector4.h>

namespace cvt {
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
