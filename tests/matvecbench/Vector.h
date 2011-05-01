#ifndef VECTOR_H
#define VECTOR_H

#include "BaseVector.h"

template<typename T>
class Vector : public BaseVector< Vector<T> >
{
	public:
		typedef T ElementType;
		typedef const Vector& StoreType;

		Vector( size_t n );
		Vector( const Vector<T>& v );
		~Vector();

		template <class DERIVED>
		Vector<T>& operator=( const BaseVector<DERIVED>& right );
		T& operator[]( size_t i ) { return _data[ i ]; }
		T operator[]( size_t i ) const { return _data[ i ]; }
		size_t size() const { return _size; }

	private:
		void alloc();

		size_t _size;
		T* _data;
};

template<typename T>
inline Vector<T>::Vector( size_t n ) : _size( n ), _data( NULL )
{
	alloc();
}

template<typename T>
inline Vector<T>::~Vector()
{
	delete[] _data;
}

template<typename T>
inline void Vector<T>::alloc()
{
	if( _data )
		delete[] _data;
	_data = new T[ _size ];
}


template< typename T>
template<class DERIVED>
inline Vector<T>& Vector<T>::operator=( const BaseVector<DERIVED>& right )
{
	const DERIVED& r = right.derived();

		size_t i = 0;
        switch( _size % 16 ){
			case 0:       do{     _data[ i ] = r[ i ]; i++;
        	case 15:              _data[ i ] = r[ i ]; i++;
        	case 14:              _data[ i ] = r[ i ]; i++;
        	case 13:              _data[ i ] = r[ i ]; i++;
        	case 12:              _data[ i ] = r[ i ]; i++;
        	case 11:              _data[ i ] = r[ i ]; i++;
        	case 10:              _data[ i ] = r[ i ]; i++;
        	case 9:               _data[ i ] = r[ i ]; i++;
        	case 8:               _data[ i ] = r[ i ]; i++;
        	case 7:               _data[ i ] = r[ i ]; i++;
        	case 6:               _data[ i ] = r[ i ]; i++;
        	case 5:               _data[ i ] = r[ i ]; i++;
        	case 4:               _data[ i ] = r[ i ]; i++;
        	case 3:               _data[ i ] = r[ i ]; i++;
        	case 2:               _data[ i ] = r[ i ]; i++;
        	case 1:               _data[ i ] = r[ i ]; i++;
						  } while( i < _size  );
		}
	
	return *this;
}


#endif
