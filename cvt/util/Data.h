#ifndef CVT_DATA_H
#define CVT_DATA_H

#include <stdint.h>
#include <cvt/util/SIMD.h>

namespace cvt {
	class Data {
		public:
			Data( size_t size = 0 );
			Data( uint8_t* ptr, size_t size );
			Data( const Data& data );
			Data& operator=( const Data& data );

			size_t size() const;
			const uint8_t* ptr() const;
			uint8_t* ptr();

			void allocate( size_t size );

		private:
			uint8_t* _data;
			size_t	 _size;
	};

	inline Data::Data( size_t size ) : _data( NULL )
	{
		allocate( size );
	}

	inline Data::Data( uint8_t* ptr, size_t size ) : _data( NULL )
	{
		allocate( size );
		SIMD::instance()->Memcpy( _data, ptr, size );
	}

	inline Data::Data( const Data& data ) : _data( NULL )
	{
		if( &data == this )
			return;
		allocate( data._size );
		SIMD::instance()->Memcpy( _data, data._data , _size );
	}

	inline Data& Data::operator=( const Data& data )
	{
		allocate( data._size );
		SIMD::instance()->Memcpy( _data, data._data , _size );
		return *this;
	}

	inline size_t Data::size() const
	{
		return _size;
	}

	inline uint8_t* Data::ptr()
	{
		return _data;
	}

	inline const uint8_t* Data::ptr() const
	{
		return _data;
	}

	inline void Data::allocate( size_t size )
	{
		if( _data )
			delete[] _data;
		_size = size;

		if( size )
			_data = new uint8_t[ _size ];
		else
			_data = NULL;
	}

}

#endif
