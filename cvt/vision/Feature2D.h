#ifndef CVT_FEATURE2D_H
#define CVT_FEATURE2D_H

#include <cvt/math/Vector.h>

namespace cvt {
	template<typename T>
	struct Feature2D {
		Feature2D( T x, T y, float angle = 0.0f, float scale = 1.0f );

		Vector2<T> pt;
		float angle;
		float scale;
	};

    template <typename T>
	inline Feature2D<T>::Feature2D( T x, T y, float a, float s ) : pt( x, y ), angle( a ), scale( s )
	{
	}

	typedef Feature2D<int> Feature2Di;
	typedef Feature2D<float> Feature2Df;
}

#endif
