#ifndef CVT_DATA_H
#define CVT_DATA_H

#include <stdint.h>
#include <cvt/util/SIMD.h>
#include <cvt/math/Math.h>

namespace cvt {
	class Data {
		public:
			Data( size_t size = 0 );
			~Data();
			Data( const uint8_t* ptr, size_t size );
			Data( uint8_t* ptr, size_t size, bool copyData );
			Data( const Data& data );
			Data& operator=( const Data& data );

			size_t size() const;
			const uint8_t* ptr() const;
			uint8_t* ptr();

			void allocate( size_t size );
			void reallocate( size_t size );

		private:
			uint8_t* _data;
			size_t	 _size;
			bool     _dealloc;
	};

	inline Data::Data( size_t size ) : _data( NULL ), _dealloc( false )
	{
		allocate( size );
	}
			
	inline Data::~Data()
	{
		if( _dealloc && _data )
		   delete[]	_data;	
	}

	inline Data::Data( const uint8_t* ptr, size_t size ) : _data( NULL ), _dealloc( false )
	{
		allocate( size );
		SIMD::instance()->Memcpy( _data, ptr, size );
	}
	
	inline Data::Data( uint8_t* ptr, size_t size, bool copyData ) : _data( NULL ), _dealloc( copyData )
	{
		if( copyData ){
			allocate( size );
			SIMD::instance()->Memcpy( _data, ptr, size );
		} else {
			_data = ptr;
			_size = size;
		}
	}

	inline Data::Data( const Data& data ) : _data( NULL ), _dealloc( false )
	{
		if( &data == this )
			return;
		allocate( data._size );
		SIMD::instance()->Memcpy( _data, data._data , _size );
		_dealloc = true;
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
		if( _data && _dealloc )
			delete[] _data;
		_size = size;
		_dealloc = true;

		if( size )
			_data = new uint8_t[ _size ];
		else
			_data = NULL;
	}

	inline void Data::reallocate( size_t size )
	{
		uint8_t* newdata = new uint8_t[ size ];

		if( _data ) {
			SIMD::instance()->Memcpy( newdata, _data, Math::min( _size, size ) );
			if( _dealloc )
				delete[] _data;
		}
		_data = newdata;
		_size = size;
	}
}

#endif
