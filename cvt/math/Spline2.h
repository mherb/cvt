#ifndef CVT_SPLINE2_H
#define CVT_SPLINE2_H

#include <cvt/math/Vector.h>
#include <cvt/math/Math.h>
#include <cvt/geom/Polygon.h>

namespace cvt {
	template<typename T> class Polygon;

	template<typename T>
	class Spline2 {
		public:
			Spline2();
			Spline2( const Spline2<T>& spl );
			Spline2( T x1, T y1, T dx1, T dy1, T dx2, T dy2, T x2, T y2  );
			Spline2( const Vector2<T>& p1, const Vector2<T>& d1, const Vector2<T>& d2, const Vector2<T>& pt2 );

			Spline2<T>& operator=( const Spline2<T>& spl );

			void set( T x1, T y1, T dx1, T dy1, T dx2, T dy2, T x2, T y2  );
			void set( const Vector2<T>& p1, const Vector2<T>& d1, const Vector2<T>& d2, const Vector2<T>& pt2 );

			Vector2<T>& operator[]( int x );
			const Vector2<T>& operator[]( int x ) const;

			void samplePoint( Vector2<T>& pt, T t );
			void sampleDerivative( Vector2<T>& pt, T t );

			void addToPolygon( Polygon<T>& poly, T tolerance = ( T ) 1 / ( T ) 2 );

			void remove( T t );
			void split( Spline2<T>& out1, Spline2<T>& out2, T t ) const;
			void splitHalf( Spline2<T>& out1, Spline2<T>& out2 ) const;

			size_t inflectionPoints( T (&t)[ 2 ] ) const;
			T flattenFirst( T tolerance ) const;
			T flatten( T tolerance ) const;
			T flattenOffset( T offset, T tolerance ) const;


		private:
			Vector2<T> _pts[ 4 ];
	};

	template<typename T>
	inline Spline2<T>::Spline2()
	{
	}

	template<typename T>
	inline Spline2<T>::Spline2( const Spline2<T>& spl )
	{
		_pts[ 0 ] = spl._pts[ 0 ];
		_pts[ 1 ] = spl._pts[ 1 ];
		_pts[ 2 ] = spl._pts[ 2 ];
		_pts[ 3 ] = spl._pts[ 3 ];
	}

	template<typename T>
	Spline2<T>::Spline2( T x1, T y1, T dx1, T dy1, T dx2, T dy2, T x2, T y2  )
	{
		_pts[ 0 ].x = x1;
		_pts[ 0 ].y = y1;
		_pts[ 1 ].x = dx1;
		_pts[ 1 ].y = dy1;
		_pts[ 2 ].x = dx2;
		_pts[ 2 ].y = dy2;
		_pts[ 3 ].x = x2;
		_pts[ 3 ].y = y2;
	}

	template<typename T>
	Spline2<T>::Spline2( const Vector2<T>& p1, const Vector2<T>& d1, const Vector2<T>& d2, const Vector2<T>& p2 )
	{
		_pts[ 0 ] = p1;
		_pts[ 1 ] = d1;
		_pts[ 2 ] = d2;
		_pts[ 3 ] = p2;
	}

	template<typename T>
	inline Spline2<T>& Spline2<T>::operator=( const Spline2<T>& spl )
	{
		_pts[ 0 ] = spl._pts[ 0 ];
		_pts[ 1 ] = spl._pts[ 1 ];
		_pts[ 2 ] = spl._pts[ 2 ];
		_pts[ 3 ] = spl._pts[ 3 ];
		return *this;
	}

	template<typename T>
	void Spline2<T>::set( T x1, T y1, T dx1, T dy1, T dx2, T dy2, T x2, T y2  )
	{
		_pts[ 0 ].x = x1;
		_pts[ 0 ].y = y1;
		_pts[ 1 ].x = dx1;
		_pts[ 1 ].y = dy1;
		_pts[ 2 ].x = dx2;
		_pts[ 2 ].y = dy2;
		_pts[ 3 ].x = x2;
		_pts[ 3 ].y = y2;
	}

	template<typename T>
	void Spline2<T>::set( const Vector2<T>& p1, const Vector2<T>& d1, const Vector2<T>& d2, const Vector2<T>& p2 )
	{
		_pts[ 0 ] = p1;
		_pts[ 1 ] = d1;
		_pts[ 2 ] = d2;
		_pts[ 3 ] = p2;
	}

	template<typename T>
	const Vector2<T>& Spline2<T>::operator[]( int x ) const
	{
		return _pts[ x ];
	}

	template<typename T>
	Vector2<T>& Spline2<T>::operator[]( int x )
	{
		return _pts[ x ];
	}

	template<typename T>
	void Spline2<T>::samplePoint( Vector2<T>& pt, T t )
	{
		/* t (t (t (-p0+3 p1-3 p2+p3)+3 p0-6 p1+3 p2)-3 p0+3 p1)+p0 */
		Vector2<T> tmp;
		pt = _pts[ 1 ] - _pts[ 2 ];
		pt *= 3;
		pt += _pts[ 3 ];
		pt -= _pts[ 0 ];
		pt *= t;
		tmp = _pts[ 0 ] + _pts[ 2 ];
		tmp -= 2 * _pts[ 1 ];
		tmp *= 3;
		pt += tmp;
		pt *= t;
		tmp = _pts[ 1 ] - _pts[ 0 ];
		tmp *= 3;
		pt += tmp;
		pt *= t;
		pt += _pts[ 0 ];
	}

	template<typename T>
	void Spline2<T>::sampleDerivative( Vector2<T>& pt, T t )
	{
		/* 3 (t (t (-p0+3 p1-3 p2+p3)+2 (p0-2 p1+p2))-p0+p1) */
		Vector2<T> tmp;
		pt = _pts[ 1 ] - _pts[ 2 ];
		pt *= 3;
		pt += _pts[ 3 ];
		pt -= _pts[ 0 ];
		pt *= t;
		tmp = _pts[ 0 ] + _pts[ 2 ];
		tmp -= 2 * _pts[ 1 ];
		tmp *= 2;
		pt += tmp;
		pt *= t;
		pt += _pts[ 1 ];
		pt -= _pts[ 0 ];
		pt *= 3;
	}

	template<typename T>
	void Spline2<T>::remove( T t )
	{
		Vector2<T> dummy[ 5 ];

		/* _pts[ 3 ] stays unchanged */
		dummy[ 0 ] = _pts[ 1 ] + ( _pts[ 2 ] - _pts[ 1 ] ) * t;
		dummy[ 1 ] = _pts[ 0 ] + ( _pts[ 1 ] - _pts[ 0 ] ) * t;
		dummy[ 1 ] += ( dummy[ 0 ] - dummy[ 1 ] ) * t;

		_pts[ 2 ] += ( _pts[ 3 ] - _pts[ 2 ] ) * t;
		_pts[ 1 ] = dummy[ 0 ] + ( _pts[ 2 ] - dummy[ 0 ] ) * t;
		_pts[ 0 ] = dummy[ 1 ] + ( _pts[ 1 ] - dummy[ 1 ] ) * t;
	}

	template<typename T>
	void Spline2<T>::split( Spline2<T>& out1, Spline2<T>& out2, T t ) const
	{
		Vector2<T> dummy;

		out1._pts[ 0 ] = _pts[ 0 ]; /*first point from spl to out1*/
		out2._pts[ 3 ] = _pts[ 3 ]; /*last point from spl to out2*/
		out1._pts[ 1 ] = _pts[ 0 ] + ( _pts[ 1 ] - _pts[ 0 ] ) * t;
		dummy = _pts[ 1 ] + ( _pts[ 2 ] - _pts[ 1 ] ) * t;
		out2._pts[ 2 ] = _pts[ 2 ] + ( _pts[ 3 ] - _pts[ 2 ] ) * t;
		out1._pts[ 2 ] = out1._pts[ 1 ] + ( dummy - out1._pts[ 1 ] ) * t;
		out2._pts[ 1 ] = dummy + ( out2._pts[ 2 ] - dummy ) * t;
		out1._pts[ 3 ] = out1._pts[ 2 ] + ( out2._pts[ 1 ] - out1._pts[ 2 ] ) * t;
		out2._pts[ 0 ] = out1._pts[ 3 ];
	}

	template<typename T>
	void Spline2<T>::splitHalf( Spline2<T>& out1, Spline2<T>& out2 ) const
	{
		Vector2<T> dummy;
		T half = ( T ) 1 / ( T ) 2;

		out1._pts[ 0 ] = _pts[ 0 ]; /*first point from spl to out1*/
		out2._pts[ 3 ] = _pts[ 3 ]; /*last point from spl to out2*/
		out1._pts[ 1 ] = _pts[ 0 ] + ( _pts[ 1 ] - _pts[ 0 ] ) * half;
		dummy = _pts[ 1 ] + ( _pts[ 2 ] - _pts[ 1 ] ) * half;
		out2._pts[ 2 ] = _pts[ 2 ] + ( _pts[ 3 ] - _pts[ 2 ] ) * half;
		out1._pts[ 2 ] = out1._pts[ 1 ] + ( dummy - out1._pts[ 1 ] ) * half;
		out2._pts[ 1 ] = dummy + ( out2._pts[ 2 ] - dummy ) * half;
		out1._pts[ 3 ] = out1._pts[ 2 ] + ( out2._pts[ 1 ] - out1._pts[ 2 ] ) * half;
		out2._pts[ 0 ] = out1._pts[ 3 ];
	}

	template<typename T>
	size_t Spline2<T>::inflectionPoints( T (&t)[ 2 ] ) const
	{
		Vector2<T> a, b, c;
		size_t n;

		a = _pts[ 3 ] - _pts[ 0 ] + 3 * ( _pts[ 1 ] - _pts[ 2 ] );
		b = _pts[ 0 ] + _pts[ 2 ] - 2 * _pts[ 1 ];
		c = _pts[ 1 ] - _pts[ 0 ];

		n = Math::solveQuadratic( a.y * b.x - a.x * b.y, a.y * c.x - a.x * c.y, b.y * c.x - b.x * c.y, t );
		if( n == 1 && ( t[ 0 ] < 0 || t[ 0 ] > 1 ) ) {
			n = 0;
		} else if( n == 2 ) {
			if( t[ 0 ] < 0 || t[ 0 ] > 1 ) {
				if( t[ 1 ] < 0  || t[ 1 ] > 1 ) {
					n = 0;
				} else {
					t[ 0 ] = t[ 1 ];
					n = 1;
				}
			} else if( t[ 1 ] < 0 || t[ 1 ] > 1 ) {
				n = 1;
			} else if( t[ 0 ] > t[ 1 ] ) {
				T tmp = t[ 0 ];
				t[ 0 ] = t[ 1 ];
				t[ 1 ] = tmp;
			}
		}
		return n;
	}

/*
    Based on paper, "Fast, precise flattening of cubic Bezier path and offset curves"
    from Thomas F. Hain, Athar L. Ahmad, Sri Venkat R. Racherla, David D. Langan
 */
	template<>
	float Spline2<float>::flattenFirst( float tolerance ) const
	{
		float t;
		float s3 = ( _pts[ 3 ].x - _pts[ 0 ].x ) * ( _pts[ 1 ].y - _pts[ 0 ].y ) -
			   ( _pts[ 3 ].y - _pts[ 0 ].y ) * ( _pts[ 1 ].x - _pts[ 0 ].x );
		float d1 = _pts[ 1 ].x - _pts[ 0 ].x;
		float d2 = _pts[ 1 ].y - _pts[ 0 ].y;

		// FIXME: check for zero length
		s3 *= Math::invSqrt( d1 * d1 + d2 * d2 + 0.00001f );

		if( s3 < 0 ) s3 = -s3;
		t = Math::pow( tolerance / ( s3 + 10.0f ), 0.333333f );
//		if( t < 0 ) t = -t;
		// FIXME
//		if( t > 0.1f ) t = 0.1f;
		return t;
	}

	template<>
	double Spline2<double>::flattenFirst( double tolerance ) const
	{
		double t;
		double s3 = ( _pts[ 3 ].x - _pts[ 0 ].x ) * ( _pts[ 1 ].y - _pts[ 0 ].y ) -
			   ( _pts[ 3 ].y - _pts[ 0 ].y ) * ( _pts[ 1 ].x - _pts[ 0 ].x );
		double d1 = _pts[ 1 ].x - _pts[ 0 ].x;
		double d2 = _pts[ 1 ].y - _pts[ 0 ].y;

		// FIXME: check for zero length
		s3 *= Math::invSqrt( d1 * d1 + d2 * d2 + 0.00001 );

		if( s3 < 0 ) s3 = -s3;
		t = Math::pow( tolerance / s3, 0.333333 );
//		if( t < 0 ) t = -t;
		// FIXME
		if( t > 0.1 ) t = 0.1;
		return t;
	}


/*
    Based on paper, "Fast, precise flattening of cubic Bezier path and offset curves"
    from Thomas F. Hain, Athar L. Ahmad, Sri Venkat R. Racherla, David D. Langan
 */
	template<typename T>
	T Spline2<T>::flatten( T tolerance ) const
	{
		T t;
		T s2 = ( _pts[ 2 ].x - _pts[ 0 ].x ) * ( _pts[ 1 ].y - _pts[ 0 ].y ) -
			   ( _pts[ 2 ].y - _pts[ 0 ].y ) * ( _pts[ 1 ].x - _pts[ 0 ].x );
		T d1 = _pts[ 1 ].x - _pts[ 0 ].x;
		T d2 = _pts[ 1 ].y - _pts[ 0 ].y;

		// FIXME: check for zero length
		s2 *= Math::invSqrt( d1 * d1 + d2 * d2 + 0.00001f );
		if( s2 < 0 ) s2 = -s2;

		t = Math::sqrt( tolerance / ( ( T ) 3 * s2 ) );

		return t;
	}

/*
    Based on paper, "Fast, precise flattening of cubic Bezier path and offset curves"
    from Thomas F. Hain, Athar L. Ahmad, Sri Venkat R. Racherla, David D. Langan
 */
	template<typename T>
	T Spline2<T>::flattenOffset( T offset, T tolerance ) const
	{
		T t;
		T s2 = ( _pts[ 2 ].x - _pts[ 0 ].x ) * ( _pts[ 1 ].y - _pts[ 0 ].y ) -
			( _pts[ 2 ].y - _pts[ 0 ].y ) * ( _pts[ 1 ].x - _pts[ 0 ].x );
		T r1 = ( _pts[ 1 ].x - _pts[ 0 ].x ) * ( _pts[ 1 ].x - _pts[ 0 ].x ) -
			( _pts[ 1 ].y - _pts[ 0 ].y ) * ( _pts[ 1 ].y - _pts[ 0 ].y );
		T d1 = _pts[ 1 ].x - _pts[ 0 ].x;
		T d2 = _pts[ 1 ].y - _pts[ 0 ].y;
		T s2abs, d, e;

		// FIXME: check for zero length
		d = Math::invSqrt( d1 * d1 + d2 * d2 + 0.00001f );
		s2 *= d;
		if( s2 < 0 )
			s2abs = -s2;
		else
			s2abs = s2;
		r1 *= d;
		r1 *= r1;

		offset *= 2;
		if( offset > 0 )
			e = ( T ) 1 - ( offset * s2 ) / ( 3 * r1 );
		else
			e = ( T ) 1 + ( offset * s2 ) / ( 3 * r1 );

		t = 2 * Math::sqrt( tolerance / ( 3 * s2abs * e ) );
		//if( t > ( T ) 1 ) t = 1;
		return t;
	}

	template<typename T>
	inline void Spline2<T>::addToPolygon( Polygon<T>& poly, T tolerance )
	{
		Spline2<T> stack[ 32 ];
		Spline2<T>* spl = stack;
		stack[ 0 ] = *this;

		poly.addPoint( _pts[ 0 ] );

		while( spl >= stack ) {
			Vector2<T> delta = ( *spl )[ 3 ] - ( *spl )[ 0 ];
			T len = Math::abs( delta[ 0 ] ) + Math::abs( delta[ 0 ] );
			T dist;
			if (len > ( T ) 1) {
				dist = Math::abs( delta[ 0 ] * ( ( *spl )[ 1 ].y - ( *spl )[ 2 ].y ) - delta[ 1 ] * ( ( *spl )[ 1 ].x - ( *spl )[ 2 ].x ) )
					 + Math::abs( delta[ 0 ] * ( ( *spl )[ 1 ].y - ( *spl )[ 3 ].y ) - delta[ 1 ] * ( ( *spl )[ 1 ].x - ( *spl )[ 3 ].x ) );
			} else {
				dist = Math::abs( ( *spl )[ 1 ].x - ( *spl )[ 2 ].x ) + Math::abs( ( *spl )[ 1 ].y - ( *spl )[ 2 ].y )
					 + Math::abs( ( *spl )[ 1 ].x - ( *spl )[ 3 ].x ) + Math::abs( ( *spl )[ 1 ].y - ( *spl )[ 3 ].y );
				len = 1;
			}
			if ( dist < tolerance * len || spl == stack + 31) {
				poly.addPoint( ( *spl )[ 3 ] );
				spl--;
			} else {
				spl->splitHalf( *( spl + 1 ), *spl );
				spl++;
			}
		}
	}

	typedef Spline2<float> Spline2f;
}

#endif
