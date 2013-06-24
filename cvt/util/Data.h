/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_DATA_H
#define CVT_DATA_H

#include <stdint.h>
#include <stdlib.h>

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

			void assign( const uint8_t* data, size_t size );

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
		assign( ptr, size );
	}

	inline Data::Data( uint8_t* ptr, size_t size, bool copyData ) : _data( NULL ), _dealloc( copyData )
	{
		if( copyData ){
			allocate( size );
			assign( ptr, size );
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
		assign( data._data, data._size );
		_dealloc = true;
	}

	inline Data& Data::operator=( const Data& data )
	{
		allocate( data._size );
		assign( data._data, data._size );
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


}

#endif
