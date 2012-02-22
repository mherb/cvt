/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_VECTOR_H
#define CVT_VECTOR_H

#include <cvt/util/SIMD.h>
#include <cvt/util/Exception.h>
#include <stdlib.h>

#include <cvt/math/MatVec/VectorExpr.h>
#include <cvt/math/MatVec/VectorScalarExpr.h>
#include <cvt/math/MatVec/VectorVectorExpr.h>

namespace cvt {
	template<typename T>
		class Vector : public VectorExpr< Vector<T> > {
			public:
				Vector( size_t size );
				Vector( const Vector& v );
				~Vector();

				Vector<T>& operator=( const Vector& other );

				template<typename DERIVED>
				Vector<T>& operator=( const VectorExpr<DERIVED>& other );

				size_t	size() const;
				void    resize( size_t size );
				void	reset();

				bool	isAliased( const Vector& o ) const;

				const T operator[]( size_t i ) const;
				T& operator[]( size_t i );

				T	eval( size_t i ) const { return _data[ i ]; }

				Vector<T>& fill( T value );

				typedef T ELEMENT_TYPE;
				typedef const Vector<T>& STORE_TYPE;
			private:
				size_t _size;
				T*	   _data;
		};

	template<typename T>
	inline Vector<T>::Vector( size_t size ) : _data( NULL )
	{
		resize( size );
	}

	template<typename T>
	inline Vector<T>::Vector( const Vector& v ) : _data( NULL )
	{
		resize( v._size );
		SIMD::instance()->Memcpy( ( uint8_t* ) _data, ( uint8_t* ) v._data, sizeof( T ) * _size );
	}

	template<typename T>
	inline Vector<T>::~Vector()
	{
		if( _data )
			free( _data );
	}

	template<typename T>
	inline Vector<T>& Vector<T>::operator=( const Vector& other )
	{
		if( this != &other ) {
			resize( other._size );
			SIMD::instance()->Memcpy( ( uint8_t* ) _data, ( uint8_t* ) other._data, sizeof( T ) * _size );
		}
		return *this;
	}


	template<typename T>
	template<typename DERIVED>
	inline Vector<T>& Vector<T>::operator=( const VectorExpr<DERIVED>& other )
	{
		const DERIVED& r = other.derived();
		if( r.size() != _size )
			throw CVTException( "Can't assign vector expressions with different size!" );
		for( size_t i = 0, end = r.size(); i < end; ++i )
			_data[ i ] = r.eval( i );
		return *this;
	}

	template<typename T>
	inline size_t Vector<T>::size() const
	{
		return _size;
	}

	template<typename T>
	inline void Vector<T>::resize( size_t size )
	{
		if( _data )
			free( _data );
		_size = size;
		if( posix_memalign( ( void** ) &_data, 16, sizeof( T ) * size ) ) {
			throw CVTException( "Out of memory!" );
		}
	}


	template<typename T>
	inline Vector<T>& Vector<T>::fill( T value )
	{
		for( size_t i = 0; i < _size; i++ )
			_data[ i ] = value;
		return *this;
	}

	template<>
	inline Vector<float>& Vector<float>::fill( float value )
	{
		SIMD::instance()->SetValue1f( _data, value, _size );
		return *this;
	}

/*	template<>
	inline Vector<double>& Vector<double>::fill( double value )
	{
		SIMD::instance()->SetValue1d( _data, value, _size );
		return *this;
	}*/

	template<typename T>
	inline void Vector<T>::reset()
	{
		if( _data )
			free( _data );
		_size = 0;
		_data = NULL;
	}

	template<typename T>
	inline const T Vector<T>::operator[]( size_t i ) const
	{
		//FIXME: add assert
		return _data[ i ];
	}

	template<typename T>
	inline T& Vector<T>::operator[]( size_t i )
	{
		//FIXME: add assert
		return _data[ i ];
	}

	template<typename T>
	inline bool	Vector<T>::isAliased( const Vector& o ) const
	{
		return ( this == &o );
	}
}


#endif
