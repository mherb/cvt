#ifndef RECT_H
#define RECT_H

#include <iostream>
#include <cvt/math/Math.h>
#include <cvt/math/Vector.h>

namespace cvt {


	template<typename T> class Rect;

	template< typename T>
		class Rect
		{
			public:
				Rect( T rx = 0, T ry = 0, T rw = 0, T rh = 0 ) : x( rx ), y( ry ), width( rw ), height( rh ) {};
				Rect( const Rect<T>& r2 );
				Rect<T>& operator=( const Rect<T>& r2 );
				void copy( const Rect<T>& r2 );
				void set( T x, T y, T width, T height );
				void setPosition( T x, T y );
				void setSize( T w, T h );
				void translate( T x, T y );
				T size() const;
				void getPosition( T& x, T& y ) const;
				void getSize( T& w, T& h ) const;
				bool contains( const T px, const T py ) const;
				bool contains( const Vector2<T>& pt ) const;
				bool contains( T x, T y, T width, T height ) const;
				bool contains( const Rect<T>& r2 ) const;
				bool intersects( const Rect<T>& r2 ) const;
				bool intersects( T x, T y, T w, T h ) const;
				void intersect( const Rect<T>& r2 );
				void intersect( T x, T y, T w, T h );
				void join( const Rect<T>& r2 );
				void join( T x, T y, T w, T h );
				void join( const Vector2<T>& pt );
				void join( T x, T y );
				bool isEmpty() const;
				bool operator==( const Rect<T>& r2 ) const;

				T x, y, width, height;
		};

	template<typename T>
		inline std::ostream& operator<<(std::ostream &out, const Rect<T> &r)
		{
			out << r.x << " " << r.y << "  " << r.width << " x " << r.height;
			return out;
		}

	typedef Rect<double> Rectd;
	typedef Rect<float> Rectf;
	typedef Rect<int> Recti;

	template<typename T>
		Rect<T>::Rect( const Rect<T>& r2 )
		{
			x = r2.x;
			y = r2.y;
			width = r2.width;
			height = r2.height;
		}

	template<typename T>
	inline Rect<T>& Rect<T>::operator=( const Rect<T>& r2 )
	{
		x = r2.x;
		y = r2.y;
		width = r2.width;
		height = r2.height;
		return *this;
	}

	template<typename T>
		inline void Rect<T>::copy( const Rect<T>& r2 )
		{
			x = r2.x;
			y = r2.y;
			width = r2.width;
			height = r2.height;
		}

	template<typename T>
		inline T Rect<T>::size() const
		{
			return width * height;
		}

	template<typename T>
		inline void Rect<T>::set( const T x, const T y, const T w, const T h )
		{
			this->x = x;
			this->y = y;
			this->width = w;
			this->height = h;
		}

	template<typename T>
		inline void Rect<T>::setPosition( T x, T y )
		{
			this->x = x;
			this->y = y;
		}

	template<typename T>
		inline void Rect<T>::setSize( T w, T h )
		{
			this->width = w;
			this->height = h;
		}

	template<typename T>
		inline void Rect<T>::translate( T x, T y )
		{
			this->x += x;
			this->y += y;
		}

	template<typename T>
		inline void Rect<T>::getPosition( T& x, T& y ) const
		{
			x = this->x;
			y = this->y;
		}

	template<typename T>
		inline void Rect<T>::getSize( T& w, T& h ) const
		{
			w = this->width;
			h = this->height;
		}

	template<typename T>
		inline bool Rect<T>::contains( const T px, const T py ) const
		{
			if( px >= x && px < x + width &&
			   py >= y && py < y + height )
				return true;
			return false;
		}


	template<typename T>
		inline bool Rect<T>::contains( const Vector2<T>& pt ) const
		{
			if( pt.x >= x && pt.x < x + width &&
			   pt.y >= y && pt.y < y + height )
				return true;
			return false;
		}

	template<typename T>
		inline bool Rect<T>::contains( const Rect<T>& r2 ) const
		{
			return contains( r2.x, r2.y, r2.width, r2.height );
		}

	template<typename T>
		inline bool Rect<T>::contains( T x2, T y2, T w2, T h2 ) const
		{
			return ( x2 >= x && x2 + w2 <= x + width &&
					 y2 >= y && y2 + h2 <= y + height );
		}

	template<typename T>
		inline bool Rect<T>::intersects( const Rect<T>& r2 ) const
		{
			return intersects( r2.x, r2.y, r2.width, r2.height );
		}


	template<typename T>
		inline bool Rect<T>::intersects( T x2, T y2, T w2, T h2 ) const
		{
			return !( x2 >= x + width || x2 + w2 <= x ||
					  y2 >= y + height || y2 + h2 <= y );
		}

	template<typename T>
		inline void Rect<T>::intersect( const Rect<T>& r2 )
		{
			intersect( r2.x, r2.y, r2.width, r2.height );
		}

	template<typename T>
		inline void Rect<T>::intersect( T x2, T y2, T w2, T h2 )
		{
			if( !intersects( x2, y2, w2, h2 ) ) {
				set( 0, 0, 0, 0 );
				return;
			}

			T xend, yend;

			xend = Math::min( x + width, x2 + w2 );
			yend = Math::min( y + height, y2 + h2 );
			x = Math::max( x, x2 );
			y = Math::max( y, y2 );
			width = xend - x;
			height = yend - y;
		}

	template<typename T>
		inline void Rect<T>::join( const Rect<T>& r2 )
		{
			T xend, yend;

			xend = Math::max( x + width, r2.x + r2.width );
			yend = Math::max( y + height, r2.y + r2.height );
			x = Math::min( x, r2.x );
			y = Math::min( y, r2.y );
			width = xend - x;
			height = yend - y;
		}

	template<typename T>
		inline void Rect<T>::join( T x2, T y2, T w2, T h2 )
		{
			T xend, yend;

			xend = Math::max( x + width, x2 + w2 );
			yend = Math::max( y + height, y2 + h2 );
			x = Math::min( x, x2 );
			y = Math::min( y, y2 );
			width = xend - x;
			height = yend - y;
		}

	template<typename T>
	inline void Rect<T>::join( const Vector2<T>& pt )
	{
		T xend, yend;

		xend = Math::max( x + width, pt.x );
		yend = Math::max( y + height, pt.y );
		x = Math::min( x, pt.x );
		y = Math::min( y, pt.y );
		width = xend - x;
		height = yend - y;
	}

	template<typename T>
	inline void Rect<T>::join( T px, T py )
	{
		T xend, yend;

		xend = Math::max( x + width, px );
		yend = Math::max( y + height, py );
		x = Math::min( x, px );
		y = Math::min( y, py );
		width = xend - x;
		height = yend - y;
	}


	template<typename T>
		inline bool Rect<T>::isEmpty() const
		{
			return ( width == 0 || height == 0 );
		}


	template<typename T>
		inline bool Rect<T>::operator==( const Rect<T>& r2 ) const
		{
			return ( x == r2.x && y == r2.y && width == r2.width && height == r2.height );
		}


}
#endif
