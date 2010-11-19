#ifdef CVT_MATRIX_H

namespace cvt {
	template<typename T> class Matrix2;

	template<typename T>
	std::ostream& operator<<( std::ostream& out, const Matrix2<T>& m );

    template<typename T>
	class Matrix2 {
	    public:
		Matrix2<T>( void );
		explicit Matrix2<T>( const Vector2<T>& x, const Vector2<T>& y );
		explicit Matrix2<T>( const T a, const T b, const T c, const T d );
		explicit Matrix2<T>( const T src[ 2 ][ 2 ] );

		const Vector2<T>&	operator[]( int index ) const;
		Vector2<T>&		operator[]( int index );
		Matrix2<T>		operator-() const;
		Matrix2<T>		operator*( const T c ) const;
		Matrix2<T>		operator+( const T c ) const;
		Matrix2<T>		operator-( const T c ) const;
		Vector2<T>		operator*( const Vector2<T> &vec ) const;
		Matrix2<T>		operator*( const Matrix2<T>& m ) const;
		Matrix2<T>		operator+( const Matrix2<T>& m ) const;
		Matrix2<T>		operator-( const Matrix2<T>& m ) const;
		Matrix2<T>&		operator*=( const T c );
		Matrix2<T>&		operator+=( const T c );
		Matrix2<T>&		operator-=( const T c );
		Matrix2<T>&		operator*=( const Matrix2<T>& m );
		Matrix2<T>&		operator+=( const Matrix2<T>& m );
		Matrix2<T>&		operator-=( const Matrix2<T>& m );


		bool			operator==( const Matrix2<T> &m ) const;
		bool			operator!=( const Matrix2<T> &m ) const;

		void			zero( void );
		void			identity( void );
		bool			isIdentity( ) const;
		bool			isSymmetric( ) const;
		bool			isDiagonal( ) const;

		T				trace( void ) const;
		T				determinant( void ) const;
		Matrix2<T>		transpose( void ) const;
		Matrix2<T>&		transposeSelf( void );
		Matrix2<T>		inverse( void ) const;
		bool			inverseSelf( void );
		Matrix2<T>		inverseFast( void ) const;
		bool			inverseFastSelf( void );

		int				dimension( void ) const;

		friend std::ostream& operator<< <>( std::ostream& out, const Matrix2<T>& m );
	    private:
		Vector2<T>		mat[ 2 ];
	};

	template<typename T>
	inline Matrix2<T>::Matrix2()
	{
	    mat[ 0 ].zero();
	    mat[ 1 ].zero();
	}

	template<typename T>
	inline Matrix2<T>::Matrix2( const T a, const T b, const T c, const T d )
	{
	    mat[ 0 ].x = a;
	    mat[ 0 ].y = b;
	    mat[ 1 ].x = c;
	    mat[ 1 ].y = d;
	}

	template<typename T>
	inline Matrix2<T>::Matrix2( const T src[ 2 ][ 2 ] )
	{
	    mat[ 0 ].x = src[ 0 ][ 0 ];
	    mat[ 0 ].y = src[ 0 ][ 1 ];
	    mat[ 1 ].x = src[ 1 ][ 0 ];
	    mat[ 1 ].y = src[ 1 ][ 1 ];
	}

	template<typename T>
	inline const Vector2<T>& Matrix2<T>::operator[]( int index ) const
	{
	    return mat[ index ];
	}

	template<typename T>
	inline Vector2<T>& Matrix2<T>::operator[]( int index )
	{
	    return mat[ index ];
	}

	template<typename T>
	inline Matrix2<T> Matrix2<T>::operator-( ) const
	{
	    return Matrix2<T>( -mat[ 0 ][ 0 ], -mat[ 0 ][ 1 ],
			       -mat[ 1 ][ 0 ], -mat[ 1 ][ 1 ] );
	}

	template<typename T>
	inline Matrix2<T> Matrix2<T>::operator*( const T c  ) const
	{
	    return Matrix2<T>( mat[ 0 ] * c, mat[ 1 ] * c );
	}

	template<typename T>
	inline Matrix2<T> Matrix2<T>::operator+( const T c  ) const
	{
	    return Matrix2<T>( mat[ 0 ] + c, mat[ 1 ] + c );
	}

	template<typename T>
	inline Matrix2<T> Matrix2<T>::operator-( const T c  ) const
	{
	    return Matrix2<T>( mat[ 0 ] - c, mat[ 1 ] - c );
	}

	template<typename T>
	inline Vector2<T> Matrix2<T>::operator*( const Vector2<T>& vec  ) const
	{
	    return Vector2<T>( mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y,
			       mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y );
	}

	template<typename T>
	inline Matrix2<T> Matrix2<T>::operator*( const Matrix2<T>& m  ) const
	{
	    return Matrix2<T>( mat[ 0 ].x * m[ 0 ].x + mat[ 0 ].y * m[ 1 ].x,
			       mat[ 0 ].x * m[ 0 ].y + mat[ 0 ].y * m[ 1 ].y,
			       mat[ 1 ].x * m[ 0 ].x + mat[ 1 ].y * m[ 1 ].x,
			       mat[ 1 ].x * m[ 0 ].y + mat[ 1 ].y * m[ 1 ].y );
	}

	template<typename T>
	inline Matrix2<T> Matrix2<T>::operator+( const Matrix2<T>& m  ) const
	{
	    return Matrix2<T>( mat[ 0 ] + m[ 0 ], mat[ 1 ] + m[ 1 ] );
	}

	template<typename T>
	inline Matrix2<T> Matrix2<T>::operator-( const Matrix2<T>& m  ) const
	{
	    return Matrix2<T>( mat[ 0 ] - m[ 0 ], mat[ 1 ] - m[ 1 ] );
	}

	template<typename T>
	inline Matrix2<T>& Matrix2<T>::operator*=( const T c  )
	{
	    mat[ 0 ] *= c;
	    mat[ 1 ] *= c;
	    return *this;
	}

	template<typename T>
	inline Matrix2<T>& Matrix2<T>::operator+=( const T c  )
	{
	    mat[ 0 ] += c;
	    mat[ 1 ] += c;
	    return *this;
	}

	template<typename T>
	inline Matrix2<T>& Matrix2<T>::operator-=( const T c  )
	{
	    mat[ 0 ] -= c;
	    mat[ 1 ] -= c;
	    return *this;
	}

	template<typename T>
	inline Matrix2<T>& Matrix2<T>::operator*=( const Matrix2<T>& m  )
	{
	    float x, y;
	    x = mat[ 0 ].x;
	    y = mat[ 0 ].y;
	    mat[ 0 ].x = x * m[ 0 ].x + y * m[ 1 ].x;
	    mat[ 0 ].y = x * m[ 0 ].y + y * m[ 1 ].y;
	    x = mat[ 1 ].x;
	    y = mat[ 1 ].y;
	    mat[ 1 ].x = x * m[ 0 ].x + y * m[ 1 ].x;
	    mat[ 1 ].y = x * m[ 0 ].y + y * m[ 1 ].y;
	    return *this;
	}

	template<typename T>
	inline Matrix2<T>& Matrix2<T>::operator+=( const Matrix2<T>& m  )
	{
	    mat[ 0 ] += m[ 0 ];
	    mat[ 1 ] += m[ 1 ];
	    return *this;
	}

	template<typename T>
	inline Matrix2<T>& Matrix2<T>::operator-=( const Matrix2<T>& m  )
	{
	    mat[ 0 ] -= m[ 0 ];
	    mat[ 1 ] -= m[ 1 ];
	    return *this;
	}

	template<typename T>
	inline bool Matrix2<T>::operator==( const Matrix2<T> &m ) const
	{
		return mat[ 0 ] == m[ 0 ] && mat[ 1 ] == m[ 1 ];
	}

	template<typename T>
	inline bool Matrix2<T>::operator!=( const Matrix2<T> &m ) const
	{
		return mat[ 0 ] != m[ 0 ] || mat[ 1 ] != m[ 1 ];
	}


	template<typename T>
	inline void Matrix2<T>::zero()
	{
	    mat[ 0 ].zero();
	    mat[ 1 ].zero();
	}

	template<>
	inline void Matrix2<float>::identity()
	{
	    mat[ 0 ].x = 1.0f;
	    mat[ 0 ].y = 0.0f;
	    mat[ 1 ].x = 0.0f;
	    mat[ 1 ].y = 1.0f;
	}

	template<>
	inline void Matrix2<double>::identity()
	{
	    mat[ 0 ].x = 1.0;
	    mat[ 0 ].y = 0.0;
	    mat[ 1 ].x = 0.0;
	    mat[ 1 ].y = 1.0;
	}

	template<>
	inline bool Matrix2<double>::isIdentity() const
	{
		return mat[ 0 ] == Vector2<double>( 1.0, 0.0 ) && mat[ 1 ] == Vector2<double>( 0.0, 1.0 );
	}

	template<>
	inline bool Matrix2<float>::isIdentity() const
	{
		return mat[ 0 ] == Vector2<float>( 1.0f, 0.0f ) && mat[ 1 ] == Vector2<float>( 0.0f, 1.0f );
	}

	template<typename T>
	inline bool Matrix2<T>::isSymmetric() const
	{
		return mat[ 0 ].y == mat[ 1 ].x;
	}

	template<>
	inline bool Matrix2<float>::isDiagonal() const
	{
		return Math::abs( mat[ 0 ].y ) < Math::EPSILONF && Math::abs( mat[ 1 ].x ) < Math::EPSILONF;
	}

	template<>
	inline bool Matrix2<double>::isDiagonal() const
	{
		return Math::abs( mat[ 0 ].y ) < Math::EPSILOND && Math::abs( mat[ 1 ].x ) < Math::EPSILOND;
	}

	template<typename T>
	inline T Matrix2<T>::trace() const
	{
		return mat[ 0 ].x + mat[ 1 ].y;
	}


	template<typename T>
	int	Matrix2<T>::dimension( void ) const
	{
		return 2;
	}

	template<typename T>
	inline std::ostream& operator<<( std::ostream& out, const Matrix2<T>& m )
	{
		out << " | " << m.mat[ 0 ].x << " " << m.mat[ 0 ].y << " | " << std::endl;
		out << " | " << m.mat[ 1 ].x << " " << m.mat[ 1 ].y << " | ";
		return out;
	}

	typedef Matrix2<float> Matrix2f;
	typedef Matrix2<double> Matrix2d;

}


#endif
