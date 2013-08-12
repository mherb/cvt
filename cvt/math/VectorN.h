#ifndef CVT_VECTORN_H
#define CVT_VECTORN_H

#include <string.h>

namespace cvt {

	template<size_t N, typename T>
		class Vector {
			public:
				Vector();
				Vector( const Vector<N,T>& other );

				T			dot( const Vector<N,T>& vec ) const;

				T			operator[]( size_t i ) const;
				T&			operator[]( size_t i );

				bool		operator==( const Vector<N,T> &v ) const;
				bool		operator!=( const Vector<N,T> &v ) const;

				T			length( void ) const;
				T			lengthSqr( void ) const;
				T			normalize( void );		// returns length
				void		clamp( const Vector<N,T>& min, const Vector<N,T>& max );

				size_t		dimension( void ) const;
				size_t		rows() const;
				size_t		cols() const;

				bool        isEqual( const Vector<N,T> & other, T epsilon ) const;

				const T*	ptr( void ) const;
				T*			ptr( void );

				void		mix( const Vector<N,T> &v1, const Vector<N,T> &v2, float alpha );

			private:
				T _vec[ N ];
		};

}

#endif
