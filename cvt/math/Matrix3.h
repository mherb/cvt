#ifdef CVT_MATRIX_H

#include <cvt/util/SIMD.h>

namespace cvt {
	template<typename T> class Matrix3;

	template<typename T>
	std::ostream& operator<<( std::ostream& out, const Matrix3<T>& m );

    template<typename T>
	class Matrix3 {
	    public:
							Matrix3<T>( void );
		explicit			Matrix3<T>( const Vector3<T>& x, const Vector3<T>& y, const Vector3<T>& z );
		explicit			Matrix3<T>( const T a, const T b, const T c,
									    const T d, const T e, const T f,
									    const T g, const T h, const T i );
		explicit			Matrix3<T>( const T src[ 3 ][ 3 ] );

		const Vector3<T>&	operator[]( int index ) const;
		Vector3<T>&			operator[]( int index );
		Matrix3<T>			operator-() const;
		Matrix3<T>			operator*( const T c ) const;
		Matrix3<T>			operator+( const T c ) const;
		Matrix3<T>			operator-( const T c ) const;
		Vector3<T>			operator*( const Vector3<T> &vec ) const;
		Matrix3<T>			operator*( const Matrix3<T>& m ) const;
		Matrix3<T>			operator+( const Matrix3<T>& m ) const;
		Matrix3<T>			operator-( const Matrix3<T>& m ) const;
		Matrix3<T>&			operator*=( const T c );
		Matrix3<T>&			operator+=( const T c );
		Matrix3<T>&			operator-=( const T c );
		Matrix3<T>&			operator*=( const Matrix3<T>& m );
		Matrix3<T>&			operator+=( const Matrix3<T>& m );
		Matrix3<T>&			operator-=( const Matrix3<T>& m );

		bool				operator==( const Matrix3<T> &m ) const;
		bool				operator!=( const Matrix3<T> &m ) const;

		void				zero( void );
		void				identity( void );
		bool				isIdentity( ) const;
		bool				isSymmetric( ) const;
		bool				isDiagonal( ) const;

		T					trace( void ) const;
		T					determinant( void ) const;
		Matrix3<T>			transpose( void ) const;
		Matrix3<T>&			transposeSelf( void );
		Matrix3<T>			inverse( void ) const;
		bool				inverseSelf( void );

		int					dimension( void ) const;
		const T*			ptr( void ) const;
		T*					ptr( void );


		friend std::ostream& operator<< <>( std::ostream& out, const Matrix3<T>& m );

	    private:
		Vector3<T>			mat[ 3 ];
	};

	template<typename T>
	inline Matrix3<T>::Matrix3()
	{
	    mat[ 0 ].zero();
	    mat[ 1 ].zero();
	    mat[ 2 ].zero();
	}

	template<typename T>
	inline Matrix3<T>::Matrix3( const Vector3<T>& x, const Vector3<T>& y, const Vector3<T>& z )
	{
	    mat[ 0 ] = x;
	    mat[ 1 ] = y;
	    mat[ 2 ] = z;
	}

	template<typename T>
	inline Matrix3<T>::Matrix3( const T a, const T b, const T c,
								const T d, const T e, const T f,
								const T g, const T h, const T i )
	{
	    mat[ 0 ].x = a;
	    mat[ 0 ].y = b;
	    mat[ 0 ].z = c;

	    mat[ 1 ].x = d;
	    mat[ 1 ].y = e;
	    mat[ 1 ].z = f;

	    mat[ 2 ].x = g;
	    mat[ 2 ].y = h;
	    mat[ 2 ].z = i;
	}

	template<typename T>
	inline Matrix3<T>::Matrix3( const T src[ 3 ][ 3 ] )
	{
		SIMD* simd = SIMD::get();
		simd->Memcpy( this->ptr(), src, sizeof( T ) * 9 );
	}

	template<typename T>
	inline const Vector3<T>& Matrix3<T>::operator[]( int index ) const
	{
	    return mat[ index ];
	}

	template<typename T>
	inline Vector3<T>& Matrix3<T>::operator[]( int index )
	{
	    return mat[ index ];
	}

	template<typename T>
	inline Matrix3<T> Matrix3<T>::operator-( ) const
	{
	    return Matrix3<T>( -mat[ 0 ], -mat[ 1 ], -mat[ 2 ] );
	}

	template<typename T>
	inline Matrix3<T> Matrix3<T>::operator*( const T c  ) const
	{
	    return Matrix3<T>( mat[ 0 ] * c, mat[ 1 ] * c, mat[ 2 ] * c );
	}

	template<typename T>
	inline Matrix3<T> Matrix3<T>::operator+( const T c  ) const
	{
	    return Matrix3<T>( mat[ 0 ] + c, mat[ 1 ] + c, mat[ 2 ] + c );
	}

	template<typename T>
	inline Matrix3<T> Matrix3<T>::operator-( const T c  ) const
	{
	    return Matrix3<T>( mat[ 0 ] - c, mat[ 1 ] - c, mat[ 2 ] - c );
	}

	template<typename T>
	inline Vector3<T> Matrix3<T>::operator*( const Vector3<T>& vec  ) const
	{
	    return Vector3<T>( mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y + mat[ 0 ].z * vec.z,
						   mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y + mat[ 1 ].z * vec.z,
						   mat[ 2 ].x * vec.x + mat[ 2 ].y * vec.y + mat[ 2 ].z * vec.z );
	}

	template<typename T>
	inline Matrix3<T> Matrix3<T>::operator*( const Matrix3<T>& m  ) const
	{
	    return Matrix3<T>( mat[ 0 ][ 0 ] * m[ 0 ][ 0 ] + mat[ 0 ][ 1 ] * m[ 1 ][ 0 ] + mat[ 0 ][ 2 ] * m[ 2 ][ 0 ],
						   mat[ 0 ][ 0 ] * m[ 0 ][ 1 ] + mat[ 0 ][ 1 ] * m[ 1 ][ 1 ] + mat[ 0 ][ 2 ] * m[ 2 ][ 1 ],
						   mat[ 0 ][ 0 ] * m[ 0 ][ 2 ] + mat[ 0 ][ 1 ] * m[ 1 ][ 2 ] + mat[ 0 ][ 2 ] * m[ 2 ][ 2 ],
						   mat[ 1 ][ 0 ] * m[ 0 ][ 0 ] + mat[ 1 ][ 1 ] * m[ 1 ][ 0 ] + mat[ 1 ][ 2 ] * m[ 2 ][ 0 ],
						   mat[ 1 ][ 0 ] * m[ 0 ][ 1 ] + mat[ 1 ][ 1 ] * m[ 1 ][ 1 ] + mat[ 1 ][ 2 ] * m[ 2 ][ 1 ],
						   mat[ 1 ][ 0 ] * m[ 0 ][ 2 ] + mat[ 1 ][ 1 ] * m[ 1 ][ 2 ] + mat[ 1 ][ 2 ] * m[ 2 ][ 2 ],
						   mat[ 2 ][ 0 ] * m[ 0 ][ 0 ] + mat[ 2 ][ 1 ] * m[ 1 ][ 0 ] + mat[ 2 ][ 2 ] * m[ 2 ][ 0 ],
						   mat[ 2 ][ 0 ] * m[ 0 ][ 1 ] + mat[ 2 ][ 1 ] * m[ 1 ][ 1 ] + mat[ 2 ][ 2 ] * m[ 2 ][ 1 ],
						   mat[ 2 ][ 0 ] * m[ 0 ][ 2 ] + mat[ 2 ][ 1 ] * m[ 1 ][ 2 ] + mat[ 2 ][ 2 ] * m[ 2 ][ 2 ] );
	}


	template<typename T>
	inline Matrix3<T> Matrix3<T>::operator+( const Matrix3<T>& m  ) const
	{
	    return Matrix3<T>( mat[ 0 ] + m[ 0 ], mat[ 1 ] + m[ 1 ], mat[ 2 ] + m[ 2 ] );
	}

	template<typename T>
	inline Matrix3<T> Matrix3<T>::operator-( const Matrix3<T>& m  ) const
	{
	    return Matrix3<T>( mat[ 0 ] - m[ 0 ], mat[ 1 ] - m[ 1 ], mat[ 2 ] - m[ 2 ] );
	}

	template<typename T>
	inline Matrix3<T>& Matrix3<T>::operator*=( const T c  )
	{
	    mat[ 0 ] *= c;
	    mat[ 1 ] *= c;
	    mat[ 2 ] *= c;
	    return *this;
	}

	template<typename T>
	inline Matrix3<T>& Matrix3<T>::operator+=( const T c  )
	{
	    mat[ 0 ] += c;
	    mat[ 1 ] += c;
	    mat[ 2 ] += c;
	    return *this;
	}

	template<typename T>
	inline Matrix3<T>& Matrix3<T>::operator-=( const T c  )
	{
	    mat[ 0 ] -= c;
	    mat[ 1 ] -= c;
	    mat[ 2 ] -= c;
	    return *this;
	}

	template<typename T>
	inline Matrix3<T>& Matrix3<T>::operator+=( const Matrix3<T>& m  )
	{
	    mat[ 0 ] += m[ 0 ];
	    mat[ 1 ] += m[ 1 ];
	    mat[ 2 ] += m[ 2 ];
	    return *this;
	}

	template<typename T>
	inline Matrix3<T>& Matrix3<T>::operator-=( const Matrix3<T>& m  )
	{
	    mat[ 0 ] -= m[ 0 ];
	    mat[ 1 ] -= m[ 1 ];
	    mat[ 2 ] -= m[ 2 ];
	    return *this;
	}

	template<typename T>
	inline Matrix3<T>&  Matrix3<T>::operator*=( const Matrix3<T>& m )
	{
		*this = (*this) * m;
		return *this;
	}

	template<typename T>
	inline bool Matrix3<T>::operator==( const Matrix3<T> &m ) const
	{
		return mat[ 0 ] == m[ 0 ] && mat[ 1 ] == m[ 1 ] && mat[ 2 ] == m[ 2 ];
	}

	template<typename T>
	inline bool Matrix3<T>::operator!=( const Matrix3<T> &m ) const
	{
		return mat[ 0 ] != m[ 0 ] || mat[ 1 ] != m[ 1 ] || mat[ 2 ] != m[ 2 ];
	}


	template<typename T>
	inline void Matrix3<T>::zero()
	{
	    mat[ 0 ].zero();
	    mat[ 1 ].zero();
		mat[ 2 ].zero();
	}

	template<>
	inline void Matrix3<float>::identity()
	{
	    mat[ 0 ].x = 1.0f;
	    mat[ 0 ].y = 0.0f;
		mat[ 0 ].z = 0.0f;

	    mat[ 1 ].x = 0.0f;
	    mat[ 1 ].y = 1.0f;
	    mat[ 1 ].z = 0.0f;

	    mat[ 2 ].x = 0.0f;
	    mat[ 2 ].y = 0.0f;
	    mat[ 2 ].z = 1.0f;
	}

	template<>
	inline void Matrix3<double>::identity()
	{
		mat[ 0 ].x = 1.0;
	    mat[ 0 ].y = 0.0;
		mat[ 0 ].z = 0.0;

	    mat[ 1 ].x = 0.0;
	    mat[ 1 ].y = 1.0;
	    mat[ 1 ].z = 0.0;

	    mat[ 2 ].x = 0.0;
	    mat[ 2 ].y = 0.0;
	    mat[ 2 ].z = 1.0;
	}

	template<>
	inline bool Matrix3<double>::isIdentity() const
	{
		return mat[ 0 ] == Vector3<double>( 1.0, 0.0, 0.0 )
			&& mat[ 1 ] == Vector3<double>( 0.0, 1.0, 0.0 )
			&& mat[ 2 ] == Vector3<double>( 0.0, 0.0, 1.0 );
	}

	template<>
	inline bool Matrix3<float>::isIdentity() const
	{
		return mat[ 0 ] == Vector3<float>( 1.0f, 0.0f, 0.0f )
			&& mat[ 1 ] == Vector3<float>( 0.0f, 1.0f, 0.0f )
			&& mat[ 2 ] == Vector3<float>( 0.0f, 0.0f, 1.0f );
	}

	template<>
	inline bool Matrix3<float>::isSymmetric() const
	{
		return Math::abs( mat[ 0 ][ 1 ] - mat[ 1 ][ 0 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 0 ][ 2 ] - mat[ 2 ][ 0 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 1 ][ 2 ] - mat[ 2 ][ 1 ] ) < Math::EPSILONF;
	}

	template<>
	inline bool Matrix3<double>::isSymmetric() const
	{
		return Math::abs( mat[ 0 ][ 1 ] - mat[ 1 ][ 0 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 0 ][ 2 ] - mat[ 2 ][ 0 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 1 ][ 2 ] - mat[ 2 ][ 1 ] ) < Math::EPSILOND;
	}


	template<>
	inline bool Matrix3<float>::isDiagonal() const
	{
		return Math::abs( mat[ 0 ][ 1 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 0 ][ 2 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 1 ][ 0 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 1 ][ 2 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 2 ][ 0 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 2 ][ 1 ] ) < Math::EPSILONF;
	}

	template<>
	inline bool Matrix3<double>::isDiagonal() const
	{
		return Math::abs( mat[ 0 ][ 1 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 0 ][ 2 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 1 ][ 0 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 1 ][ 2 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 2 ][ 0 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 2 ][ 1 ] ) < Math::EPSILOND;
	}

	template<typename T>
	inline T Matrix3<T>::trace() const
	{
		return mat[ 0 ].x + mat[ 1 ].y + mat[ 2 ].z;
	}

	template<typename T>
	inline T Matrix3<T>::determinant() const
	{
		T cofactor[ 3 ];

		cofactor[ 0 ] = mat[ 1 ][ 1 ] * mat[ 2 ][ 2 ] - mat[ 2 ][ 1 ] * mat[ 1 ][ 2 ];
		cofactor[ 1 ] = mat[ 2 ][ 0 ] * mat[ 1 ][ 2 ] - mat[ 1 ][ 0 ] * mat[ 2 ][ 2 ];
		cofactor[ 2 ] = mat[ 1 ][ 0 ] * mat[ 2 ][ 1 ] - mat[ 2 ][ 0 ] * mat[ 1 ][ 1 ];

		return mat[ 0 ][ 0 ] * cofactor[ 0 ] + mat[ 0 ][ 1 ] * cofactor[ 1 ] + mat[ 0 ][ 2 ] * cofactor[ 2 ];
	}


	template<typename T>
	inline Matrix3<T> Matrix3<T>::transpose() const
	{
		return Matrix3<T>( mat[ 0 ][ 0 ], mat[ 1 ][ 0 ], mat[ 2 ][ 0 ],
						   mat[ 0 ][ 1 ], mat[ 1 ][ 1 ], mat[ 2 ][ 1 ],
						   mat[ 0 ][ 2 ], mat[ 1 ][ 2 ], mat[ 2 ][ 2 ] );
	}

	template<typename T>
	inline Matrix3<T>& Matrix3<T>::transposeSelf()
	{
		float tmp;
		tmp = mat[ 0 ][ 1 ];
		mat[ 0 ][ 1 ] = mat[ 1 ][ 0 ];
		mat[ 1 ][ 0 ] = tmp;

		tmp = mat[ 0 ][ 2 ];
		mat[ 0 ][ 2 ] = mat[ 2 ][ 0 ];
		mat[ 2 ][ 0 ] = tmp;

		tmp = mat[ 1 ][ 2 ];
		mat[ 1 ][ 2 ] = mat[ 2 ][ 1 ];
		mat[ 2 ][ 1 ] = tmp;

		return *this;
	}


	template<typename T>
	inline Matrix3<T> Matrix3<T>::inverse( void ) const
	{
		Matrix3<T> inv;

		inv = *this;
		inv.inverseSelf();
		return inv;
	}

	template<typename T>
	int	Matrix3<T>::dimension( void ) const
	{
		return 3;
	}

    template<typename T>
	const T* Matrix3<T>::ptr( void ) const
	{
	    return mat[ 0 ].ptr();
	}

    template<typename T>
	T* Matrix3<T>::ptr( void )
	{
	    return mat[ 0 ].ptr();
	}

	template<typename T>
	inline std::ostream& operator<<( std::ostream& out, const Matrix3<T>& m )
	{
		out << " | " << m[ 0 ].x << " " << m[ 0 ].y << " " << m[ 0 ].z << " | " << std::endl;
		out << " | " << m[ 1 ].x << " " << m[ 1 ].y << " " << m[ 1 ].z << " | " << std::endl;
		out << " | " << m[ 2 ].x << " " << m[ 2 ].y << " " << m[ 2 ].z << " | ";
		return out;
	}

	typedef Matrix3<float> Matrix3f;
	typedef Matrix3<double> Matrix3d;

}


#endif
