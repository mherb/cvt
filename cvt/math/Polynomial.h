#ifndef CVT_POLYNOMIAL_H
#define CVT_POLYNOMIAL_H

#include <cvt/math/Math.h>

namespace cvt {
	template<typename T>
	class Polynomial {
			Polynomial( size_t degree );
			Polynomial( T a, T b );
			Polynomial( T a, T b, T c );
			Polynomial( T a, T b, T c, T d );
			Polynomial( T a, T b, T c, T d, T e );
			Polynomial( const Polynomial<T>& p );
			~Polynomial();

			Polynomial<T>& operator=( const Polynomial<T>& p );
			T			   operator[]( size_t index ) const;
			T&			   operator[]( size_t index );

			Polynomial<T>  operator-() const;

			Polynomial<T>  operator+( const Polynomial<T>& p ) const;
			Polynomial<T>  operator-( const Polynomial<T>& p ) const;
			Polynomial<T>  operator*( T s ) const;
			Polynomial<T>  operator/( T s ) const;

			Polynomial<T>& operator+=( const Polynomial<T>& p );
			Polynomial<T>& operator-=( const Polynomial<T>& p );
			Polynomial<T>& operator*=( T s );
			Polynomial<T>& operator/=( T s );

			void		   setZero();
			size_t		   degree() const;
			T			   eval( T x ) const;
			T			   operator()( T x ) const;

			Polynomial<T>  derivative() const;
			Polynomial<T>  antiDerivative() const;

			const T*	   ptr() const;
			T*			   ptr();

		private:
			void resize( size_t degree, bool keep = false );

			size_t  _degree;
			size_t  _size;
			T*		_coeff;
	};

	template<typename T>
	inline Polynomial<T>::Polynomial( size_t degree )
	{
		resize( degree );
	}

	template<typename T>
	inline Polynomial<T>::Polynomial( T a, T b )
	{
		resize( 1 );
		_coeff[ 1 ] = a;
		_coeff[ 0 ] = b;
	}

	template<typename T>
	inline Polynomial<T>::Polynomial( T a, T b, T c )
	{
		resize( 2 );
		_coeff[ 2 ] = a;
		_coeff[ 1 ] = b;
		_coeff[ 0 ] = c;
	}

	template<typename T>
	inline Polynomial<T>::Polynomial( T a, T b, T c, T d )
	{
		resize( 3 );
		_coeff[ 3 ] = a;
		_coeff[ 2 ] = b;
		_coeff[ 1 ] = c;
		_coeff[ 0 ] = d;
	}

	template<typename T>
	inline Polynomial<T>::Polynomial( T a, T b, T c, T d, T e )
	{
		resize( 4 );
		_coeff[ 4 ] = a;
		_coeff[ 3 ] = b;
		_coeff[ 2 ] = c;
		_coeff[ 1 ] = d;
		_coeff[ 0 ] = e;
	}

	template<typename T>
	inline Polynomial<T>::~Polynomial()
	{
		if( _coeff )
			delete[] _coeff;
	}

	template<typename T>
	inline Polynomial<T>& operator=( const Polynomial<T>& p )
	{
		if( this == &p ) return *this;

		resize( p._degree );
		for( size_t i = 0; i <= _degree; i++ )
			_coeff[ i ] = p._coeff[ i ];
		return *this;
	}

	template<typename T>
	inline T Polynomial<T>::operator[]( size_t index ) const
	{
		return _coeff[ index ];
	}

	template<typename T>
	inline T& Polynomial<T>::operator[]( size_t index )
	{
		return _coeff[ index ];
	}

	template<typename T>
	inline Polynomial<T> Polynomial<T>::operator-() const
	{
		Polynomial<T> ret( _degree );
		for( size_t i = 0; i <= _degree; i++ )
			ret._coeff[ i ] = - _coeff[ i ];
		return ret;
	}

	template<typename T>
	inline Polynomial<T> Polynomial<T>::operator+( const Polynomial<T>& p ) const
	{
		if( _degree > p._degree ) {
			Polynomial<T> ret( _degree );
			size_t i;
			for( i = 0; i <= p._degree; i++ )
				ret._coeff[ i ] = _coeff[ i ] + p._coeff[ i ];
			for( ; i <= _degree; i++ )
				ret._coeff[ i ] = _coeff[ i ];
			return ret;
		} else if( _degree < p._degree ) {
			Polynomial<T> ret( p._degree );
			size_t i;
			for( i = 0; i <= _degree; i++ )
				ret._coeff[ i ] = _coeff[ i ] + p._coeff[ i ];
			for( ; i <= p._degree; i++ )
				ret._coeff[ i ] = p._coeff[ i ];
			return ret;
		} else {
			Polynomial<T> ret( _degree );
			for( i = 0; i <= _degree; i++ )
				ret._coeff[ i ] = _coeff[ i ] + p._coeff[ i ];
			return ret;
		}
	}

	template<typename T>
	inline Polynomial<T> Polynomial<T>::operator-( const Polynomial<T>& p ) const
	{
		if( _degree > p._degree ) {
			Polynomial<T> ret( _degree );
			size_t i;
			for( i = 0; i <= p._degree; i++ )
				ret._coeff[ i ] = _coeff[ i ] - p._coeff[ i ];
			for( ; i <= _degree; i++ )
				ret._coeff[ i ] = _coeff[ i ];
			return ret;
		} else if( _degree < p._degree ) {
			Polynomial<T> ret( p._degree );
			size_t i;
			for( i = 0; i <= _degree; i++ )
				ret._coeff[ i ] = _coeff[ i ] - p._coeff[ i ];
			for( ; i <= p._degree; i++ )
				ret._coeff[ i ] = p._coeff[ i ];
			return ret;
		} else {
			Polynomial<T> ret( _degree );
			for( i = 0; i <= _degree; i++ ) {
				ret._coeff[ i ] = _coeff[ i ] - p._coeff[ i ];
				if( ret._coeff[ i ] != 0 )
					ret._degree = i;
			}
			return ret;
		}
	}

	template<typename T>
	inline Polynomial<T> Polynomial<T>::operator*( T s ) const
	{
		Polynomial<T> ret( _degree );
		for( size_t i = 0; i <= _degree; i++ ) {
			ret._coeff[ i ] = s * _coeff[ i ];
			if( ret._coeff[ i ] != 0 )
				ret._degree = i;
		}
		return ret;
	}

	template<typename T>
	inline Polynomial<T> Polynomial<T>::operator/( T s ) const
	{
		Polynomial<T> ret( _degree );
		if( s == 0 ) {
			ret._coeff[ 0 ] = 0;
			ret._degree = 0;
		} else {
			T inv = ( T ) 1 / s;
			for( size_t i = 0; i <= _degree; i++ ) {
				ret._coeff[ i ] = inv * _coeff[ i ];
				if( ret._coeff[ i ] != 0 )
					ret._degree = i;
			}
		}
		return ret;
	}

	template<typename T>
	inline Polynomial<T>& Polynomial<T>::operator+=( const Polynomial<T>& p )
	{
		if( _degree > p._degree ) {
			for( size_t i = 0; i <= p._degree; i++ )
				_coeff[ i ] += p._coeff[ i ];
			return *this;
		} else if( _degree < p._degree ) {
			size_t i;

			resize( p._degree, true );
			for( i = 0; i <= _degree; i++ )
				_coeff[ i ] += p._coeff[ i ];
			for( ; i <= p._degree; i++ )
				_coeff[ i ] = p._coeff[ i ];
			return *this;
		} else {
			for( size_t i = 0; i <= _degree; i++ )
				_coeff[ i ] += p._coeff[ i ];
			return *this;
		}
	}

	template<typename T>
	inline Polynomial<T>& Polynomial<T>::operator-=( const Polynomial<T>& p )
	{
		if( _degree > p._degree ) {
			for( size_t i = 0; i <= p._degree; i++ )
				_coeff[ i ] -= p._coeff[ i ];
			return *this;
		} else if( _degree < p._degree ) {
			size_t i;

			resize( p._degree, true );
			for( i = 0; i <= _degree; i++ )
				_coeff[ i ] -= p._coeff[ i ];
			for( ; i <= p._degree; i++ )
				_coeff[ i ] = -p._coeff[ i ];
			return *this;
		} else {
			for( size_t i = 0; i <= _degree; i++ )
				_coeff[ i ] -= p._coeff[ i ];
			return *this;
		}
	}

	template<typename T>
	inline Polynomial<T>& Polynomial<T>::operator*=( T s )
	{
		for( size_t i = 0; i <= _degree; i++ ) {
			_coeff[ i ] *= s;
			if( _coeff[ i ] != 0 )
				_degree = i;
		}
		return *this;
	}

	template<typename T>
	inline Polynomial<T>& Polynomial<T>::operator/=( T s )
	{
		if( s == 0 ) {
			_coeff[ 0 ] = 0;
			_degree = 0;
		} else {
			T inv = ( T ) 1 / s;
			for( size_t i = 0; i <= _degree; i++ ) {
				_coeff[ i ] *= inv;
				if( _coeff[ i ] != 0 )
					_degree = i;
			}
		}
		return *this;
	}

	template<typename T>
	inline void Polynomial<T>::setZero()
	{
		resize( 0 );
		_coeff[ 0 ] = 0;
	}

	template<typename T>
	inline size_t Polynomial<T>::degree()
	{
		return _degree;
	}

	template<typename T>
	inline T Polynomial<T>::eval( T x ) const
	{
		T y = 0;
		for( size_t i = 0; i < _degree; i++ )
			y = ( y + _coeff[ i ] ) * x;
		y += _coeff[ _degree ];
		return y;
	}

	template<typename T>
	inline T Polynomial<T>::operator()( T x ) const
	{
		T y = 0;
		for( size_t i = 0; i < _degree; i++ )
			y = ( y + _coeff[ i ] ) * x;
		y += _coeff[ _degree ];
		return y;
	}

	template<typename T>
	inline Polynomial<T> Polynomial<T>::derivative() const
	{
		Polynomial<T> ret( _degree - 1 );
		for( size_t i = 1; i <= _degree; i++ )
			ret._coeff[ i - 1 ] = ( ( T ) i ) * _coeff[ i ];
		return ret;
	}

	template<typename T>
	inline Polynomial<T> Polynomial<T>::antiDerivative() const
	{
		Polynomial<T> ret( _degree + 1 );

		ret._coeff[ 0 ] = 0;
		for( size_t i = 0; i <= _degree; i++ )
			ret._coeff[ i + 1 ] = _coeff[ i ] / ( ( T ) ( i+ 1 ) );
		return ret;
	}

	template<typename T>
	inline const T* Polynomial<T>::ptr() const
	{
		return _coeff;
	}

	template<typename T>
	inline T* Polynomial<T>::ptr()
	{
		return _coeff;
	}

	template<typename T>
	inline void Polynomial<T>::resize( size_t degree, bool keep )
	{
		if( _size < degree + 1) {
			_size = Math::pad16( degree + 1 );
			T* ncoeff = new T[ _size ];
			if( _coeff ) {
				if( keep ) {
					for( size_t i = 0; i <= _degree; i++ )
						ncoeff[ i ] = _coeff[ i ];
				}
				delete[] _coeff;
			}
			_coeff = ncoeff;
		}
		_degree = degree;
	}
}

#endif
