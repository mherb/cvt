#ifndef RECT_H
#define RECT_H

#include <iostream>
#include <cvt/math/Math.h>

namespace cvt {


	template<typename T> class Rect;

	template<typename T>
		inline std::ostream& operator<<(std::ostream &out, const Rect<T> &r);

	template< typename T>
		class Rect
		{
			friend std::ostream& operator<< <>(std::ostream &os, const Rect<T> &r );

			public:
			Rect( T rx = 0, T ry = 0, T rw = 1, T rh = 1 ) : x( rx ), y( ry ), width( rw ), height( rh ) {};
			Rect( const Rect<T>& r2 );
			void set( const T x, const T y, const T width, const T height );
			void setPosition( const T x, const T y );
			void setSize( const T w, const T h );
			T size() const;
			void getPosition( T& x, T& y ) const;
			void getSize( T& w, T& h ) const;
			bool contains( const T px, const T py ) const;
			bool contains( T x, T y, T width, T height ) const;
			bool contains( const Rect<T>& r2 ) const;
			bool intersects( const Rect<T>& r2 ) const;
			bool intersects( T x, T y, T w, T h ) const;
			void intersect( const Rect<T>& r2 );
			void intersect( T x, T y, T w, T h );

			T x, y, width, height;
		};

	template<typename T>
		inline std::ostream& operator<<(std::ostream &out, const Rect<T> &r)
		{
			return out << r.x << " " << r.y << "  " << r.width << " x " << r.height << std::endl;
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
			if( px >= x && px <= x + width &&
			   py >= y && py <= y + height )
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
			if( !contains( x2 + w2, y2 + h2 ) )
				return false;
			if( !contains( x2, y2 ) )
				return false;
			return true;
		}

	template<typename T>
		inline bool Rect<T>::intersects( const Rect<T>& r2 ) const
		{
			return intersects( r2.x, r2.y, r2.width, r2.height );
		}


	template<typename T>
		inline bool Rect<T>::intersects( T x2, T y2, T w2, T h2 ) const
		{
			if( x2 + w2 < x || x2 > x + width )
				return false;
			if( y2 + h2 < y || y2 > y + width )
				return false;
			return true;
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

}
#endif
