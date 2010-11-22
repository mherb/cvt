#ifdef CVT_MATRIX_H

#include <cvt/util/SIMD.h>

namespace cvt {
	template<typename T> class Matrix4;

	template<typename T>
	std::ostream& operator<<( std::ostream& out, const Matrix4<T>& m );

    template<typename T>
	class Matrix4 {
	    public:
							Matrix4<T>( void );
		explicit			Matrix4<T>( const Vector4<T>& x, const Vector4<T>& y, const Vector4<T>& z, const Vector4<T>& w );
		explicit			Matrix4<T>( const T a, const T b, const T c, const T d,
									    const T e, const T f, const T g, const T h,
										const T i, const T j, const T k, const T l,
										const T m, const T n, const T o, const T p );
		explicit			Matrix4<T>( const T src[ 4 ][ 4 ] );

		const Vector4<T>&	operator[]( int index ) const;
		Vector4<T>&			operator[]( int index );
		Matrix4<T>			operator-() const;
		Matrix4<T>			operator*( const T c ) const;
		Matrix4<T>			operator+( const T c ) const;
		Matrix4<T>			operator-( const T c ) const;
		Vector4<T>			operator*( const Vector4<T> &vec ) const;
		Matrix4<T>			operator*( const Matrix4<T>& m ) const;
		Matrix4<T>			operator+( const Matrix4<T>& m ) const;
		Matrix4<T>			operator-( const Matrix4<T>& m ) const;
		Matrix4<T>&			operator*=( const T c );
		Matrix4<T>&			operator+=( const T c );
		Matrix4<T>&			operator-=( const T c );
		Matrix4<T>&			operator*=( const Matrix4<T>& m );
		Matrix4<T>&			operator+=( const Matrix4<T>& m );
		Matrix4<T>&			operator-=( const Matrix4<T>& m );

		bool				operator==( const Matrix4<T> &m ) const;
		bool				operator!=( const Matrix4<T> &m ) const;

		void				zero( void );
		void				identity( void );
		bool				isIdentity( ) const;
		bool				isSymmetric( ) const;
		bool				isDiagonal( ) const;

		T					trace( void ) const;
		T					determinant( void ) const;
		Matrix4<T>			transpose( void ) const;
		Matrix4<T>&			transposeSelf( void );
		Matrix4<T>			inverse( void ) const;
		bool				inverseSelf( void );

		int					dimension( void ) const;
		const T*			ptr( void ) const;
		T*					ptr( void );


		friend std::ostream& operator<< <>( std::ostream& out, const Matrix4<T>& m );

	    private:
		Vector4<T>			mat[ 4 ];
	};

	template<typename T>
	inline Matrix4<T>::Matrix4()
	{
	    mat[ 0 ].zero();
	    mat[ 1 ].zero();
	    mat[ 2 ].zero();
	    mat[ 3 ].zero();
	}

	template<typename T>
	inline Matrix4<T>::Matrix4( const Vector4<T>& x, const Vector4<T>& y, const Vector4<T>& z, const Vector4<T>& w )
	{
	    mat[ 0 ] = x;
	    mat[ 1 ] = y;
	    mat[ 2 ] = z;
	    mat[ 3 ] = w;
	}

	template<typename T>
	inline	Matrix4<T>::Matrix4( const T a, const T b, const T c, const T d,
								 const T e, const T f, const T g, const T h,
								 const T i, const T j, const T k, const T l,
								 const T m, const T n, const T o, const T p )
	{
	    mat[ 0 ].x = a;
	    mat[ 0 ].y = b;
	    mat[ 0 ].z = c;
	    mat[ 0 ].w = d;

	    mat[ 1 ].x = e;
	    mat[ 1 ].y = f;
	    mat[ 1 ].z = g;
	    mat[ 1 ].w = h;

	    mat[ 2 ].x = i;
	    mat[ 2 ].y = j;
	    mat[ 2 ].z = k;
	    mat[ 2 ].w = l;

	    mat[ 3 ].x = m;
	    mat[ 3 ].y = n;
	    mat[ 3 ].z = o;
	    mat[ 3 ].w = p;
	}

	template<typename T>
	inline Matrix4<T>::Matrix4( const T src[ 4 ][ 4 ] )
	{
		SIMD* simd = SIMD::get();
		simd->Memcpy( this->ptr(), src, sizeof( T ) * 16 );
	}

	template<typename T>
	inline const Vector4<T>& Matrix4<T>::operator[]( int index ) const
	{
	    return mat[ index ];
	}

	template<typename T>
	inline Vector4<T>& Matrix4<T>::operator[]( int index )
	{
	    return mat[ index ];
	}

	template<typename T>
	inline Matrix4<T> Matrix4<T>::operator-( ) const
	{
	    return Matrix4<T>( -mat[ 0 ], -mat[ 1 ], -mat[ 2 ], -mat[ 3 ] );
	}

	template<typename T>
	inline Matrix4<T> Matrix4<T>::operator*( const T c  ) const
	{
	    return Matrix4<T>( mat[ 0 ] * c, mat[ 1 ] * c, mat[ 2 ] * c, mat[ 3 ] * c );
	}

	template<typename T>
	inline Matrix4<T> Matrix4<T>::operator+( const T c  ) const
	{
	    return Matrix4<T>( mat[ 0 ] + c, mat[ 1 ] + c, mat[ 2 ] + c, mat[ 3 ] + c );
	}

	template<typename T>
	inline Matrix4<T> Matrix4<T>::operator-( const T c  ) const
	{
	    return Matrix4<T>( mat[ 0 ] - c, mat[ 1 ] - c, mat[ 2 ] - c, mat[ 3 ] - c );
	}

	template<typename T>
	inline Vector4<T> Matrix4<T>::operator*( const Vector4<T>& vec  ) const
	{
	    return Vector4<T>( mat[ 0 ].x * vec.x + mat[ 0 ].y * vec.y + mat[ 0 ].z * vec.z, mat[ 0 ].w * vec.w,
						   mat[ 1 ].x * vec.x + mat[ 1 ].y * vec.y + mat[ 1 ].z * vec.z, mat[ 1 ].w * vec.w,
						   mat[ 2 ].x * vec.x + mat[ 2 ].y * vec.y + mat[ 2 ].z * vec.z, mat[ 2 ].w * vec.w,
						   mat[ 3 ].x * vec.x + mat[ 3 ].y * vec.y + mat[ 3 ].z * vec.z, mat[ 3 ].w * vec.w );
	}

	template<typename T>
	inline Matrix4<T> Matrix4<T>::operator*( const Matrix4<T>& m  ) const
	{
	    return Matrix4<T>( mat[ 0 ][ 0 ] * m[ 0 ][ 0 ] + mat[ 0 ][ 1 ] * m[ 1 ][ 0 ] + mat[ 0 ][ 2 ] * m[ 2 ][ 0 ] + mat[ 0 ][ 3 ] * m[ 3 ][ 0 ],
						   mat[ 0 ][ 0 ] * m[ 0 ][ 1 ] + mat[ 0 ][ 1 ] * m[ 1 ][ 1 ] + mat[ 0 ][ 2 ] * m[ 2 ][ 1 ] + mat[ 0 ][ 3 ] * m[ 3 ][ 1 ],
						   mat[ 0 ][ 0 ] * m[ 0 ][ 2 ] + mat[ 0 ][ 1 ] * m[ 1 ][ 2 ] + mat[ 0 ][ 2 ] * m[ 2 ][ 2 ] + mat[ 0 ][ 3 ] * m[ 3 ][ 2 ],
						   mat[ 0 ][ 0 ] * m[ 0 ][ 3 ] + mat[ 0 ][ 1 ] * m[ 1 ][ 3 ] + mat[ 0 ][ 2 ] * m[ 2 ][ 3 ] + mat[ 0 ][ 3 ] * m[ 3 ][ 3 ],

						   mat[ 1 ][ 0 ] * m[ 0 ][ 0 ] + mat[ 1 ][ 1 ] * m[ 1 ][ 0 ] + mat[ 1 ][ 2 ] * m[ 2 ][ 0 ] + mat[ 1 ][ 3 ] * m[ 3 ][ 0 ],
						   mat[ 1 ][ 0 ] * m[ 0 ][ 1 ] + mat[ 1 ][ 1 ] * m[ 1 ][ 1 ] + mat[ 1 ][ 2 ] * m[ 2 ][ 1 ] + mat[ 1 ][ 3 ] * m[ 3 ][ 1 ],
						   mat[ 1 ][ 0 ] * m[ 0 ][ 2 ] + mat[ 1 ][ 1 ] * m[ 1 ][ 2 ] + mat[ 1 ][ 2 ] * m[ 2 ][ 2 ] + mat[ 1 ][ 3 ] * m[ 3 ][ 2 ],
						   mat[ 1 ][ 0 ] * m[ 0 ][ 3 ] + mat[ 1 ][ 1 ] * m[ 1 ][ 3 ] + mat[ 1 ][ 2 ] * m[ 2 ][ 3 ] + mat[ 1 ][ 3 ] * m[ 3 ][ 3 ],

						   mat[ 2 ][ 0 ] * m[ 0 ][ 0 ] + mat[ 2 ][ 1 ] * m[ 1 ][ 0 ] + mat[ 2 ][ 2 ] * m[ 2 ][ 0 ] + mat[ 2 ][ 3 ] * m[ 3 ][ 0 ],
						   mat[ 2 ][ 0 ] * m[ 0 ][ 1 ] + mat[ 2 ][ 1 ] * m[ 1 ][ 1 ] + mat[ 2 ][ 2 ] * m[ 2 ][ 1 ] + mat[ 2 ][ 3 ] * m[ 3 ][ 1 ],
						   mat[ 2 ][ 0 ] * m[ 0 ][ 2 ] + mat[ 2 ][ 1 ] * m[ 1 ][ 2 ] + mat[ 2 ][ 2 ] * m[ 2 ][ 2 ] + mat[ 2 ][ 3 ] * m[ 3 ][ 2 ],
						   mat[ 2 ][ 0 ] * m[ 0 ][ 3 ] + mat[ 2 ][ 1 ] * m[ 1 ][ 3 ] + mat[ 2 ][ 2 ] * m[ 2 ][ 3 ] + mat[ 2 ][ 3 ] * m[ 3 ][ 3 ],

						   mat[ 3 ][ 0 ] * m[ 0 ][ 0 ] + mat[ 3 ][ 1 ] * m[ 1 ][ 0 ] + mat[ 3 ][ 2 ] * m[ 2 ][ 0 ] + mat[ 3 ][ 3 ] * m[ 3 ][ 0 ],
						   mat[ 3 ][ 0 ] * m[ 0 ][ 1 ] + mat[ 3 ][ 1 ] * m[ 1 ][ 1 ] + mat[ 3 ][ 2 ] * m[ 2 ][ 1 ] + mat[ 3 ][ 3 ] * m[ 3 ][ 1 ],
						   mat[ 3 ][ 0 ] * m[ 0 ][ 2 ] + mat[ 3 ][ 1 ] * m[ 1 ][ 2 ] + mat[ 3 ][ 2 ] * m[ 2 ][ 2 ] + mat[ 3 ][ 3 ] * m[ 3 ][ 2 ],
						   mat[ 3 ][ 0 ] * m[ 0 ][ 3 ] + mat[ 3 ][ 1 ] * m[ 1 ][ 3 ] + mat[ 3 ][ 2 ] * m[ 2 ][ 3 ] + mat[ 3 ][ 3 ] * m[ 3 ][ 3 ] );
	}

	template<typename T>
	inline Matrix4<T> Matrix4<T>::operator+( const Matrix4<T>& m  ) const
	{
	    return Matrix4<T>( mat[ 0 ] + m[ 0 ], mat[ 1 ] + m[ 1 ], mat[ 2 ] + m[ 2 ], mat[ 3 ] + m[ 3 ] );
	}

	template<typename T>
	inline Matrix4<T> Matrix4<T>::operator-( const Matrix4<T>& m  ) const
	{
	    return Matrix4<T>( mat[ 0 ] - m[ 0 ], mat[ 1 ] - m[ 1 ], mat[ 2 ] - m[ 2 ], mat[ 3 ] - m[ 3 ] );
	}

	template<typename T>
	inline Matrix4<T>& Matrix4<T>::operator*=( const T c  )
	{
	    mat[ 0 ] *= c;
	    mat[ 1 ] *= c;
	    mat[ 2 ] *= c;
	    mat[ 3 ] *= c;
	    return *this;
	}

	template<typename T>
	inline Matrix4<T>& Matrix4<T>::operator+=( const T c  )
	{
	    mat[ 0 ] += c;
	    mat[ 1 ] += c;
	    mat[ 2 ] += c;
	    mat[ 3 ] += c;
	    return *this;
	}

	template<typename T>
	inline Matrix4<T>& Matrix4<T>::operator-=( const T c  )
	{
	    mat[ 0 ] -= c;
	    mat[ 1 ] -= c;
	    mat[ 2 ] -= c;
	    mat[ 3 ] -= c;
	    return *this;
	}

	template<typename T>
	inline Matrix4<T>& Matrix4<T>::operator+=( const Matrix4<T>& m  )
	{
	    mat[ 0 ] += m[ 0 ];
	    mat[ 1 ] += m[ 1 ];
	    mat[ 2 ] += m[ 2 ];
	    mat[ 3 ] += m[ 3 ];
	    return *this;
	}

	template<typename T>
	inline Matrix4<T>& Matrix4<T>::operator-=( const Matrix4<T>& m  )
	{
	    mat[ 0 ] -= m[ 0 ];
	    mat[ 1 ] -= m[ 1 ];
	    mat[ 2 ] -= m[ 2 ];
	    mat[ 3 ] -= m[ 3 ];
	    return *this;
	}

	template<typename T>
	inline Matrix4<T>&  Matrix4<T>::operator*=( const Matrix4<T>& m )
	{
		*this = (*this) * m;
		return *this;
	}


	template<typename T>
	inline bool Matrix4<T>::operator==( const Matrix4<T> &m ) const
	{
		return mat[ 0 ] == m[ 0 ] && mat[ 1 ] == m[ 1 ] && mat[ 2 ] == m[ 2 ] && mat[ 3 ] == m[ 3 ];
	}

	template<typename T>
	inline bool Matrix4<T>::operator!=( const Matrix4<T> &m ) const
	{
		return mat[ 0 ] != m[ 0 ] || mat[ 1 ] != m[ 1 ] || mat[ 2 ] != m[ 2 ] || mat[ 3 ] != m[ 3 ];
	}


	template<typename T>
	inline void Matrix4<T>::zero()
	{
	    mat[ 0 ].zero();
	    mat[ 1 ].zero();
		mat[ 2 ].zero();
		mat[ 3 ].zero();
	}

	template<>
	inline void Matrix4<float>::identity()
	{
	    mat[ 0 ].x = 1.0f;
	    mat[ 0 ].y = 0.0f;
		mat[ 0 ].z = 0.0f;
		mat[ 0 ].w = 0.0f;

	    mat[ 1 ].x = 0.0f;
	    mat[ 1 ].y = 1.0f;
	    mat[ 1 ].z = 0.0f;
		mat[ 1 ].w = 0.0f;

	    mat[ 2 ].x = 0.0f;
	    mat[ 2 ].y = 0.0f;
	    mat[ 2 ].z = 1.0f;
		mat[ 2 ].w = 0.0f;

	    mat[ 3 ].x = 0.0f;
	    mat[ 3 ].y = 0.0f;
	    mat[ 3 ].z = 0.0f;
		mat[ 3 ].w = 1.0f;
	}

	template<>
	inline void Matrix4<double>::identity()
	{
	    mat[ 0 ].x = 1.0;
	    mat[ 0 ].y = 0.0;
		mat[ 0 ].z = 0.0;
		mat[ 0 ].w = 0.0;

	    mat[ 1 ].x = 0.0;
	    mat[ 1 ].y = 1.0;
	    mat[ 1 ].z = 0.0;
		mat[ 1 ].w = 0.0;

	    mat[ 2 ].x = 0.0;
	    mat[ 2 ].y = 0.0;
	    mat[ 2 ].z = 1.0;
		mat[ 2 ].w = 0.0;

	    mat[ 3 ].x = 0.0;
	    mat[ 3 ].y = 0.0;
	    mat[ 3 ].z = 0.0;
		mat[ 3 ].w = 1.0;
	}

	template<>
	inline bool Matrix4<double>::isIdentity() const
	{
		return mat[ 0 ] == Vector4<double>( 1.0, 0.0, 0.0, 0.0 )
			&& mat[ 1 ] == Vector4<double>( 0.0, 1.0, 0.0, 0.0 )
			&& mat[ 2 ] == Vector4<double>( 0.0, 0.0, 1.0, 0.0 )
			&& mat[ 3 ] == Vector4<double>( 0.0, 0.0, 0.0, 1.0 );
	}

	template<>
	inline bool Matrix4<float>::isIdentity() const
	{
		return mat[ 0 ] == Vector4<float>( 1.0f, 0.0f, 0.0f, 0.0f )
			&& mat[ 1 ] == Vector4<float>( 0.0f, 1.0f, 0.0f, 0.0f )
			&& mat[ 2 ] == Vector4<float>( 0.0f, 0.0f, 1.0f, 0.0f )
			&& mat[ 3 ] == Vector4<float>( 0.0f, 0.0f, 0.0f, 1.0f );
	}

	template<>
	inline bool Matrix4<float>::isSymmetric() const
	{
		return Math::abs( mat[ 0 ][ 1 ] - mat[ 1 ][ 0 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 0 ][ 2 ] - mat[ 2 ][ 0 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 0 ][ 3 ] - mat[ 3 ][ 0 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 1 ][ 2 ] - mat[ 2 ][ 1 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 1 ][ 3 ] - mat[ 3 ][ 1 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 2 ][ 3 ] - mat[ 3 ][ 2 ] ) < Math::EPSILONF;

	}

	template<>
	inline bool Matrix4<float>::isDiagonal() const
	{
		return Math::abs( mat[ 0 ][ 1 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 0 ][ 2 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 0 ][ 3 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 1 ][ 0 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 1 ][ 2 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 1 ][ 3 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 2 ][ 0 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 2 ][ 1 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 2 ][ 3 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 3 ][ 0 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 3 ][ 1 ] ) < Math::EPSILONF
			&& Math::abs( mat[ 3 ][ 2 ] ) < Math::EPSILONF;
	}

	template<>
	inline bool Matrix4<double>::isDiagonal() const
	{
		return Math::abs( mat[ 0 ][ 1 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 0 ][ 2 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 0 ][ 3 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 1 ][ 0 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 1 ][ 2 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 1 ][ 3 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 2 ][ 0 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 2 ][ 1 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 2 ][ 3 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 3 ][ 0 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 3 ][ 1 ] ) < Math::EPSILOND
			&& Math::abs( mat[ 3 ][ 2 ] ) < Math::EPSILOND;
	}

	template<typename T>
	inline T Matrix4<T>::trace() const
	{
		return mat[ 0 ].x + mat[ 1 ].y + mat[ 2 ].z + mat[ 3 ].w;
	}

	template<typename T>
	inline T Matrix4<T>::determinant() const
	{
		T det2_23_01, det2_23_02, det2_23_03,
		  det2_23_12, det2_23_13, det2_23_23;
		T det3_123_123, det3_123_023, det3_123_013, det3_123_012;

		det2_23_01 = mat[ 2 ][ 0 ] * mat[ 3 ][ 1 ] - mat[ 3 ][ 0 ] * mat[ 2 ][ 1 ];
		det2_23_02 = mat[ 2 ][ 0 ] * mat[ 3 ][ 2 ] - mat[ 3 ][ 0 ] * mat[ 2 ][ 2 ];
		det2_23_03 = mat[ 2 ][ 0 ] * mat[ 3 ][ 3 ] - mat[ 3 ][ 0 ] * mat[ 2 ][ 3 ];
		det2_23_12 = mat[ 2 ][ 1 ] * mat[ 3 ][ 2 ] - mat[ 3 ][ 1 ] * mat[ 2 ][ 2 ];
		det2_23_13 = mat[ 2 ][ 1 ] * mat[ 3 ][ 3 ] - mat[ 3 ][ 1 ] * mat[ 2 ][ 3 ];
		det2_23_23 = mat[ 2 ][ 2 ] * mat[ 3 ][ 3 ] - mat[ 3 ][ 2 ] * mat[ 2 ][ 3 ];

		det3_123_123  =  mat[ 1 ][ 1 ] * det2_23_23 - mat[ 1 ][ 2 ] * det2_23_13 + mat[ 1 ][ 3 ] * det2_23_12;
		det3_123_023  =  mat[ 1 ][ 0 ] * det2_23_23 - mat[ 1 ][ 2 ] * det2_23_03 + mat[ 1 ][ 3 ] * det2_23_02;
		det3_123_013  =  mat[ 1 ][ 0 ] * det2_23_13 - mat[ 1 ][ 1 ] * det2_23_03 + mat[ 1 ][ 3 ] * det2_23_01;
		det3_123_012  =  mat[ 1 ][ 0 ] * det2_23_12 - mat[ 1 ][ 1 ] * det2_23_02 + mat[ 1 ][ 2 ] * det2_23_01;

		return mat[ 0 ][ 0 ] * det3_123_123 - mat[ 0 ][ 1 ] * det3_123_023 + mat[ 0 ][ 2 ] * det3_123_013 - mat[ 0 ][ 3 ] * det3_123_012;
	}


	template<typename T>
	inline Matrix4<T> Matrix4<T>::transpose() const
	{
		return Matrix4<T>( mat[ 0 ][ 0 ], mat[ 1 ][ 0 ], mat[ 2 ][ 0 ], mat[ 3 ][ 0 ],
						   mat[ 0 ][ 1 ], mat[ 1 ][ 1 ], mat[ 2 ][ 1 ], mat[ 3 ][ 1 ],
						   mat[ 0 ][ 2 ], mat[ 1 ][ 2 ], mat[ 2 ][ 2 ], mat[ 3 ][ 2 ],
						   mat[ 0 ][ 3 ], mat[ 1 ][ 3 ], mat[ 2 ][ 3 ], mat[ 3 ][ 3 ] );
	}

	template<typename T>
	inline Matrix4<T>& Matrix4<T>::transposeSelf()
	{
		float tmp;
		tmp = mat[ 0 ][ 1 ];
		mat[ 0 ][ 1 ] = mat[ 1 ][ 0 ];
		mat[ 1 ][ 0 ] = tmp;

		tmp = mat[ 0 ][ 2 ];
		mat[ 0 ][ 2 ] = mat[ 2 ][ 0 ];
		mat[ 2 ][ 0 ] = tmp;

		tmp = mat[ 0 ][ 3 ];
		mat[ 0 ][ 3 ] = mat[ 3 ][ 0 ];
		mat[ 3 ][ 0 ] = tmp;

		tmp = mat[ 1 ][ 2 ];
		mat[ 1 ][ 2 ] = mat[ 2 ][ 1 ];
		mat[ 2 ][ 1 ] = tmp;

		tmp = mat[ 1 ][ 3 ];
		mat[ 1 ][ 3 ] = mat[ 3 ][ 1 ];
		mat[ 3 ][ 1 ] = tmp;

		tmp = mat[ 2 ][ 3 ];
		mat[ 2 ][ 3 ] = mat[ 3 ][ 2 ];
		mat[ 3 ][ 2 ] = tmp;

		return *this;
	}


	template<typename T>
	inline Matrix4<T> Matrix4<T>::inverse( void ) const
	{
		Matrix4<T> inv;

		inv = *this;
		inv.inverseSelf();
		return inv;
	}

	template<typename T>
	int	Matrix4<T>::dimension( void ) const
	{
		return 4;
	}

    template<typename T>
	const T* Matrix4<T>::ptr( void ) const
	{
	    return mat[ 0 ].ptr();
	}

    template<typename T>
	T* Matrix4<T>::ptr( void )
	{
	    return mat[ 0 ].ptr();
	}

	template<typename T>
	inline std::ostream& operator<<( std::ostream& out, const Matrix4<T>& m )
	{
		out << " | " << m[ 0 ].x << " " << m[ 0 ].y << " " << m[ 0 ].z << " " << m[ 0 ].w << " | " << std::endl;
		out << " | " << m[ 1 ].x << " " << m[ 1 ].y << " " << m[ 1 ].z << " " << m[ 1 ].w << " | " << std::endl;
		out << " | " << m[ 2 ].x << " " << m[ 2 ].y << " " << m[ 2 ].z << " " << m[ 2 ].w << " | " << std::endl;
		out << " | " << m[ 3 ].x << " " << m[ 3 ].y << " " << m[ 3 ].z << " " << m[ 3 ].w << " | ";
		return out;
	}

	typedef Matrix4<float> Matrix4f;
	typedef Matrix4<double> Matrix4d;

}


#endif
