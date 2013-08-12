#ifndef CVT_MATRIXNM_H
#define CVT_MATRIXNM_H

#include <iostream>

namespace cvt {

	template<typename T1, size_t N, size_t M, typename T>
	class MVExpr;


	template<size_t N, size_t M, typename T>
		class Matrix {
			public:
				Matrix();
				Matrix( const Matrix<N,M,T>& m );

				void		setIdentity();

				T			operator()( size_t row, size_t col ) const;
				T&			operator()( size_t row, size_t col );

				bool		operator==( const Matrix<N,M,T> &v ) const;
				bool		operator!=( const Matrix<N,M,T> &v ) const;

				template<typename XPR>
				Matrix<N,M,T>& operator=( const MVExpr<XPR, N, M, T>& expr );


				bool        isEqual( const Matrix<N,M,T> & other, T epsilon ) const;

				size_t		rows() const;
				size_t		cols() const;

				const T*	ptr( void ) const;
				T*			ptr( void );

			private:
				T	_mat[ N ][ M ];
		};

	template<size_t N, size_t M, typename T>
	inline Matrix<N,M,T>::Matrix()
	{
	}


	template<size_t N, size_t M, typename T>
	inline void	Matrix<N,M,T>::setIdentity()
	{
		for( size_t r = 0; r < N; r++ ) {
			for( size_t c = 0; c < M; c++ ) {
				operator()( r, c ) = ( T ) ( r==c ? 1 : 0 );
			}
		}
	}

	template<size_t N, size_t M, typename T>
	inline T Matrix<N,M,T>::operator()( size_t row, size_t col ) const
	{
		return _mat[ row ][ col ];
	}

	template<size_t N, size_t M, typename T>
	inline T& Matrix<N,M,T>::operator()( size_t row, size_t col )
	{
		return _mat[ row ][ col ];
	}

	template<size_t N, size_t M, typename T>
    inline std::ostream& operator<<( std::ostream& out, const Matrix<N,M,T>& m )
    {
		out << "Matrix<" << N << ", " << M << ">" << std::endl;
		for( size_t r = 0; r < N; r++ ) {
			for( size_t c = 0; c < M; c++ ) {
				out << m( r, c ) << " ";
			}
			out << std::endl;
		}

        return out;
    }
}

#endif
