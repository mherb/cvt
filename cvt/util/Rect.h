#ifndef RECT_H
#define RECT_H

#include <iostream>

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
	bool contains( T px, T py ) const;
	bool contains( const Rect<T> r2 ) const;
	bool intersects( Rect<T> r2 ) const;

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
inline bool Rect<T>::contains( T px, T py ) const
{
    if( px >= x && px <= x + width &&
        py >= y && py <= y + height )
	return true;
    return false;
}

template<typename T>
inline bool Rect<T>::contains( Rect<T> r2 ) const
{
    if( !contains( r2.x + r2.width, r2.y + r2.height ) )
	return false;
    if( !contains( r2.x, r2.y ) )
	return false;
    return true;
}

template<typename T>
inline bool Rect<T>::intersects( Rect<T> r2 ) const
{
    if( r2.x + r2.width < x || r2.x > x + width )
	return false;
    if( r2.y + r2.height < y || r2.y > y + width )
	return false;
    return true;
}



}
#endif
