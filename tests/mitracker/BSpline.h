#ifndef CVT_BSPLINE_H
#define CVT_BSPLINE_H

#include <cvt/math/Math.h>

namespace cvt {

	template<typename T>
	class BSpline {
		public:
			static T eval( T t );
			static T evalDerivative( T t );
			static T evalSecondDerivative( T t );
	};

	template<typename T>
	inline T BSpline<T>::eval( T t  )
	{
		t = Math::abs( t );
		if( t > 2.0f ) return 0.0f;
		if( t > 1.0f ) {
			t = ( 2.0f - t );
			return t * t * t / 6.0f;
		}
		return ( 4.0f - 6.0f * t * t + 3.0f * t * t *t ) / 6.0f;
	}

	template<typename T>
	inline T BSpline<T>::evalDerivative( T t  )
	{
		T z = Math::abs( t );
		if( z > 2.0f ) return 0.0f;
		if( t < -1.0f ) {
			t = ( 2.0f + t );
			return 3.0f * t * t / 6.0f;
		}
		if( t > 1.0f ) {
			t = ( 2.0f - t );
			return -3.0f * t * t / 6.0f;
		}
		if( t < 0.0f )
			return ( 3.0f + 6.0f * ( 1 + t ) - 9.0f * ( 1 + t ) * ( 1 + t ) ) / 6.0f;
		return -( 3.0f + 6.0f * ( 1 - t ) - 9.0f * ( 1 - t ) * ( 1 - t ) ) / 6.0f;
	}

	template<typename T>
	inline T BSpline<T>::evalSecondDerivative( T t  )
	{
		T z = Math::abs( t );
		if( z > 2.0f ) return 0.0f;
		if( t < -1.0f ) {
			t = ( 2.0f + t );
			return 6.0f * t / 6.0f;
		}
		if( t > 1.0f ) {
			t = ( 2.0f - t );
			return 6.0f * t / 6.0f;
		}
		if( t < 0.0f )
			return ( 6.0f - 18.0f * ( 1 + t ) ) / 6.0f;
		return ( 6.0f - 18.0f * ( 1 - t ) ) / 6.0f;
	}

	typedef BSpline<float> BSplinef;
	typedef BSpline<double> BSplined;
}

#endif
