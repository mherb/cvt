#ifdef CVT_VECTOR_H

namespace cvt {

    template<typename T>
	class Vector4 {
	    public:
					Vector4();
					Vector4( T x, T y, T z, T w );
					Vector4( const Vector4<T>& vec2 );

		void		set( T x, T y, T z, T w );
		void		zero( void );

		T			operator[]( int index ) const;
		T&			operator[]( int index );
		Vector4<T>	operator-() const;
		T			operator*( const Vector4<T> &v ) const;
		Vector4<T>	operator*( const T c ) const;
		Vector4<T>	cmul( const Vector4<T>& v ) const;
		Vector4<T>	operator/( const T c ) const;
		Vector4<T>	operator+( const Vector4<T> &v ) const;
		Vector4<T>	operator-( const Vector4<T> &v ) const;
		Vector4<T>&	operator+=( const Vector4<T> &v );
		Vector4<T>&	operator-=( const Vector4<T> &v );
		Vector4<T>&	operator/=( const Vector4<T> &v );
		Vector4<T>&	operator+=( const T c );
		Vector4<T>&	operator-=( const T c );
		Vector4<T>&	operator/=( const T c );
		Vector4<T>&	operator*=( const T c );

		bool		operator==( const Vector4<T> &v ) const;
		bool		operator!=( const Vector4<T> &v ) const;

		T			length( void ) const;
		T			lengthSqr( void ) const;
		T			normalize( void );		// returns length
		void		clamp( const Vector4<T> &min, const Vector4<T> &max );

		int			dimension( void ) const;

		const T*	ptr( void ) const;
		T*			ptr( void );

		void		mix( const Vector4<T> &v1, const Vector4<T> &v2, float alpha );

		T x, y, z, w;
	};

    template<typename T>
	inline Vector4<T>::Vector4()
	{
	    this->x = 0;
	    this->y = 0;
	    this->z = 0;
	    this->w = 0;
	}

    template<typename T>
	inline Vector4<T>::Vector4( T x, T y, T z, T w )
	{
	    this->x = x;
	    this->y = y;
	    this->z = z;
	    this->w = w;
	}

    template<typename T>
	inline Vector4<T>::Vector4( const Vector4<T>& v )
	{
	    x = v.x;
	    y = v.y;
	    z = v.z;
	    w = v.w;
	}

    template<typename T>
	inline void Vector4<T>::set( T x, T y, T z, T w )
	{
	    this->x = x;
	    this->y = y;
	    this->z = z;
	    this->w = w;
	}

    template<typename T>
	inline void Vector4<T>::zero( )
	{
	    x = 0;
	    y = 0;
		z = 0;
		w = 0;
	}

    template<typename T>
	inline T Vector4<T>::operator[]( int index ) const
	{
	    return ( &x )[ index ];
	}

    template<typename T>
	inline T& Vector4<T>::operator[]( int index )
	{
	    return ( &x )[ index ];
	}

    template<typename T>
	inline Vector4<T> Vector4<T>::operator-() const
	{
	    return Vector4<T>( -x, -y, -z, -w );
	}

    template<typename T>
	inline Vector4<T> operator*( float c, const Vector4<T> &v )
	{
	    return Vector4<T>( c * v.x, c * v.y, c * v.z, c * v.w );
	}

    template<typename T>
	inline Vector4<T> Vector4<T>::operator*( const T c ) const
	{
	    return Vector4<T>( x * c, y * c, z * c, w * c );
	}

    template<typename T>
	inline Vector4<T> Vector4<T>::cmul( const Vector4<T>& v ) const
	{
	    return Vector4<T>( v.x * x, v.y * y, v.z * z, v.w * w );
	}

    template<typename T>
	inline T Vector4<T>::operator*( const Vector4<T>& v ) const
	{
	    return v.x * x + v.y * y + v.z * z + v.w * w;
	}

    template<typename T>
	inline Vector4<T> Vector4<T>::operator/( const T c ) const
	{
	    T inv = ( ( T ) 1.0 ) / c;
	    return Vector4<T>( x * inv, y * inv, z * inv, w * inv );
	}

    template<>
	inline Vector4<float> Vector4<float>::operator/( const float c ) const
	{
	    float inv = 1.0f / c;
	    return Vector4<float>( x * inv, y * inv, z * inv, w * inv );
	}

    template<>
	inline Vector4<double> Vector4<double>::operator/( const double c ) const
	{
	    double inv = 1.0 / c;
	    return Vector4<double>( x * inv, y * inv, z * inv, w * inv );
	}

    template<typename T>
	inline Vector4<T> Vector4<T>::operator+( const Vector4<T> &v ) const
	{
	    return Vector4<T>( x + v.x, y + v.y, z + v.z, w + v.w );
	}

    template<typename T>
	inline Vector4<T> Vector4<T>::operator-( const Vector4<T> &v ) const
	{
	    return Vector4<T>( x - v.x, y - v.y, z - v.z, w - v.w );
	}

    template<typename T>
	inline Vector4<T>& Vector4<T>::operator+=( const Vector4<T> &v )
	{
	    x += v.x;
	    y += v.y;
		z += v.z;
		w += v.w;
	    return *this;
	}

    template<typename T>
	inline Vector4<T>& Vector4<T>::operator-=( const Vector4<T> &v )
	{
	    x -= v.x;
	    y -= v.y;
		z -= v.z;
		w -= v.w;
	    return *this;
	}

    template<typename T>
	inline Vector4<T>& Vector4<T>::operator/=( const Vector4<T> &v )
	{
	    x /= v.x;
	    y /= v.y;
	    z /= v.z;
	    w /= v.w;
	    return *this;
	}

    template<typename T>
	inline Vector4<T>& Vector4<T>::operator+=( const T c )
	{
	    x += c;
	    y += c;
		z += c;
		w += c;
	    return *this;
	}

    template<typename T>
	inline Vector4<T>& Vector4<T>::operator-=( const T c )
	{
	    x -= c;
	    y -= c;
	    z -= c;
	    w -= c;
	    return *this;
	}


    template<typename T>
	inline Vector4<T>& Vector4<T>::operator/=( const T c )
	{
	    x /= c;
	    y /= c;
	    z /= c;
	    w /= c;
	    return *this;
	}

    template<typename T>
	inline Vector4<T>& Vector4<T>::operator*=( const T c )
	{
	    x *= c;
	    y *= c;
	    z *= c;
	    w *= c;
	    return *this;
	}

	template<>
	inline bool Vector4<float>::operator==( const Vector4<float> &v ) const
	{
		return Math::abs( v.x - x ) < Math::EPSILONF
			&& Math::abs( v.y - y ) < Math::EPSILONF
			&& Math::abs( v.z - z ) < Math::EPSILONF
			&& Math::abs( v.w - w ) < Math::EPSILONF;
	}

	template<>
	inline bool Vector4<float>::operator!=( const Vector4<float> &v ) const
	{
		return Math::abs( v.x - x ) > Math::EPSILONF
			|| Math::abs( v.y - y ) > Math::EPSILONF
			|| Math::abs( v.z - z ) > Math::EPSILONF
			|| Math::abs( v.w - w ) > Math::EPSILONF;
	}

	template<>
	inline bool Vector4<double>::operator==( const Vector4<double> &v ) const
	{
		return Math::abs( v.x - x ) < Math::EPSILOND
			&& Math::abs( v.y - y ) < Math::EPSILOND
			&& Math::abs( v.z - z ) < Math::EPSILOND
			&& Math::abs( v.w - w ) < Math::EPSILOND;
	}

	template<>
	inline bool Vector4<double>::operator!=( const Vector4<double> &v ) const
	{
		return Math::abs( v.x - x ) > Math::EPSILOND
			|| Math::abs( v.y - y ) > Math::EPSILOND
			|| Math::abs( v.z - z ) > Math::EPSILOND
			|| Math::abs( v.w - w ) > Math::EPSILOND;
	}


    template<typename T>
	inline T Vector4<T>::length( void ) const
	{
	    return Math::sqrt( x * x + y * y + z * z  + w * w );
	}

    template<typename T>
	inline T Vector4<T>::lengthSqr( void ) const
	{
	    return x * x + y * y + z * z + w * w;
	}

    template<typename T>
	T Vector4<T>::normalize( void )
	{
	    T lenSqr, lenInv;

	    lenSqr = x * x + y * y + z * z + w * w;
	    lenInv = Math::invSqrt( lenSqr );
	    x *= lenInv;
	    y *= lenInv;
		z *= lenInv;
		w *= lenInv;
	    return lenSqr * lenInv;
	}

    template<typename T>
	void Vector4<T>::clamp( const Vector4<T> &min, const Vector4<T> &max )
	{
	    x = Math::clamp( x, min.x, max.x );
	    y = Math::clamp( y, min.y, max.y );
	    z = Math::clamp( z, min.z, max.z );
	    w = Math::clamp( w, min.w, max.w );
	}

    template<typename T>
	int Vector4<T>::dimension( void ) const
	{
	    return 4;
	}

    template<typename T>
	const T* Vector4<T>::ptr( void ) const
	{
	    return &x;
	}

    template<typename T>
	T* Vector4<T>::ptr( void )
	{
	    return &x;
	}

    template<typename T>
	void Vector4<T>::mix( const Vector4<T> &v1, const Vector4<T> &v2, float alpha )
	{
	    x = Math::mix( v1.x, v2.x, alpha );
	    y = Math::mix( v1.y, v2.y, alpha );
	    z = Math::mix( v1.z, v2.z, alpha );
	    w = Math::mix( v1.w, v2.w, alpha );
	}

	template<typename T>
	std::ostream& operator<<( std::ostream& out, const Vector4<T> &v )
	{
		out << " [ " << v.x << " " << v.y << " " << v.z << " " << v.w << " ] ";
		return out;
	}

	typedef Vector4<float> Vector4f;
	typedef Vector4<double> Vector4d;
}

#endif
