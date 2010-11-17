#ifndef CVT_VECTOR_H
#define CVT_VECTOR_H

#include <cvt/math/Math.h>

namespace cvt {

    template<typename T>
	class Vector2 {
	    public:
		Vector2( T x, T y );
		Vector2( const Vector2<T>& vec2 );

		void		set( const T x, const T y );
		void		zero( void );

		T		operator[]( int index ) const;
		T&		operator[]( int index );
		Vector2<T>	operator-() const;
		T		operator*( const Vector2<T> &v ) const;
		Vector2<T>	operator*( const T c ) const;
		Vector2<T>	operator/( const T c ) const;
		Vector2<T>	operator+( const Vector2<T> &v ) const;
		Vector2<T>	operator-( const Vector2<T> &v ) const;
		Vector2<T>&	operator+=( const Vector2<T> &v );
		Vector2<T>&	operator-=( const Vector2<T> &v );
		Vector2<T>&	operator/=( const Vector2<T> &v );
		Vector2<T>&	operator+=( const T c );
		Vector2<T>&	operator-=( const T c );
		Vector2<T>&	operator/=( const T c );
		Vector2<T>&	operator*=( const T c );

/*		int		compare( const Vector2<T> &v ) const;
		bool		operator==( const Vector2<T> &v ) const;
		bool		operator!=( const Vector2<T> &v ) const;*/

		T		length( void ) const;
		T		lengthSqr( void ) const;
		T		normalize( void );		// returns length
		void		clamp( const Vector2<T> &min, const Vector2<T> &max );

		int		dimension( void ) const;

		const T*	ptr( void ) const;
		T*		ptr( void );

		void		mix( const Vector2<T> &v1, const Vector2<T> &v2, float alpha );

//		union {
		    T x, y;
//		} __attribute__( ( aligned( 16 ) ) );
	};

    template<T>
	inline Vector2<T>::Vector2( T x, T y )
	{
	    this->x = x;
	    this->y = y;
	}

    template<T>
	inline Vector2<T>::Vector2( const Vector2<T>& v )
	{
	    x = v.x;
	    y = v.y;
	}

    template<T>
	inline Vector2<T>::set( T x, T y )
	{
	    this->x = x;
	    this->y = y;
	}

    template<T>
	inline Vector2<T>::zero( )
	{
	    x = 0;
	    y = 0;
	}

    template<T>
	inline T operator[]( int index ) const
	{
	    return ( &x )[ index ];
	}

    template<T>
	inline T& operator[]( int index )
	{
	    return ( &x )[ index ];
	}

    template<T>
	inline Vector2<T> operator-() const
	{
	    return Vector2<T>( -x, -y );
	}

    template<T>
	inline T operator*( const Vector2<T> &v ) const
	{
	    return x * v.x + y * v.y;
	}

    template<T>
	inline Vector2<T> operator*( const T c ) const
	{
	    return Vector2<T>( x * c, y * c );
	}

    template<T>
	inline Vector2<T> operator*( const float c, const Vector2<T> v )
	{
	    return Vector2<T>( v.x * c, v.y * a );
	}

    template<T>
	inline Vector2<T> operator/( const T c ) const
	{
	    T inv = ( ( T ) 1.0 ) / c;
	    return Vector2<T>( x * inv, y * inv );
	}

    template<T>
	inline Vector2<T> operator+( const Vector2<T> &v ) const
	{
	    return Vector2<T>( x + v.x, y + v.y );
	}

    template<T>
	inline Vector2<T> operator-( const Vector2<T> &v ) const
	{
	    return Vector2<T>( x - v.x, y - v.y );
	}

    template<T>
	inline Vector2<T>& operator+=( const Vector2<T> &v )
	{
	    x += v.x;
	    y += v.y;
	    return *this;
	}

    template<T>
	inline Vector2<T>& operator-=( const Vector2<T> &v )
	{
	    x -= v.x;
	    y -= v.y;
	    return *this;
	}

    template<T>
	inline Vector2<T>& operator/=( const Vector2<T> &v )
	{
	    x /= v.x;
	    y /= v.y;
	    return *this;
	}

    template<T>
	inline Vector2<T>& operator+=( const T c );
	{
	    x += c;
	    y += c;
	    return *this;
	}

    template<T>
	inline Vector2<T>& operator-=( const T c );
	{
	    x -= c;
	    y -= c;
	    return *this;
	}


    template<T>
	inline Vector2<T>& operator/=( const T c )
	{
	    x /= c;
	    y /= c;
	    return *this;
	}

    template<T>
	inline Vector2<T>& operator*=( const T c );
	{
	    x *= c;
	    y *= c;
	    return *this;
	}

    template<T>
	inline T Vector2<T>::length( void ) const
	{
	    return Math::sqrt( x * x + y * y );
	}

    template<T>
	inline T Vector2<T>::lengthSqr( void ) const
	{
	    return x * x + y * y;
	}

    template<T>
	T Vector2<T>::normalize( void )
	{
	    T lenSqr, lenInv;

	    lenSqr = x * x + y * y;
	    lenInv = Math::invSqrt( lenSqr );
	    x *= lenInv;
	    y *= lenInv;
	    return lenSqr * lenInv;
	}

    template<T>
	void Vector2<T>::clamp( const Vector2<T> &min, const Vector2<T> &max )
	{
	    x = Math::clamp( x, min.x, max.x );
	    y = Math::clamp( y, min.y, max.y );
	}

    template<T>
	int Vector2<T>::dimension( void ) const
	{
	    return 2;
	}

    template<T>
	const T* Vector2<T>::ptr( void ) const
	{
	    return &x;
	}

    template<T>
	T* Vector2<T>::ptr( void )
	{
	    return &x;
	}

    template<T>
	void Vector2<T>::mix( const Vector2<T> &v1, const Vector2<T> &v2, float alpha )
	{
	    x = Math::mix( v1.x, v2.x, alpha );
	    y = Math::mix( v1.y, v2.y, alpha );
	}

}


#endif
