#ifndef CVT_POLYGON2D_H
#define CVT_POLYGON2D_H

#include <cvt/math/Math.h>
#include <cvt/math/Vector.h>
#include <vector>

namespace cvt {
	template<typename T>
	class Polygon2D {
		public:
			Polygon2D(){};
			~Polygon2D(){};
			Vector2<T>& operator[]( int index );
			size_t size() const;
			void addPoint( const Vector2<T>& pt );
			T area() const;
			Vector2<T> centroid() const;

		private:
			std::vector<Vector2<T> > _pts;
	};

	template<typename T>
	inline size_t Polygon2D<T>::size() const
	{
		return _pts.size();
	}

	template<typename T>
	inline Vector2<T>& Polygon2D<T>::operator[]( int i )
	{
		return _pts[ i ];
	}

	template<typename T>
	inline void Polygon2D<T>::addPoint( const Vector2<T>& pt )
	{
		_pts.push_back( pt );
	}

	template<typename T>
	inline T Polygon2D<T>::area() const
	{
		size_t n = size() - 1;
		T area = 0;

		const Vector2<T>* pt = &_pts[ 0 ];
		while( n-- ) {
			area += pt->x * ( pt + 1 )->y - pt->y * ( pt + 1 )->x;
			pt++;
		}
		return area / 2;
	}

	template<typename T>
	inline Vector2<T> Polygon2D<T>::centroid() const
	{
		size_t n = size() - 1;
		Vector2<T> c( 0, 0 );
		T det;
		T area2 = 0;

		const Vector2<T>* pt = &_pts[ 0 ];
		while( n-- ) {
			det = pt->x * ( pt + 1 )->y - pt->y * ( pt + 1 )->x;
			area2 += det;
			c.x += ( pt->x + ( pt + 1 )->x ) * det;
			c.y += ( pt->y + ( pt + 1 )->y ) * det;
			pt++;
		}
		c /= 3 * area2;
		return c;
	}

}

#endif
