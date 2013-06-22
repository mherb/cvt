#include <cvt/util/Data.h>
#include <cvt/util/SIMD.h>
#include <cvt/math/Math.h>

namespace cvt {

	void Data::allocate( size_t size )
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

	void Data::reallocate( size_t size )
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


	void Data::assign( const uint8_t* data, size_t size )
	{
		SIMD::instance()->Memcpy( _data, data , Math::min( size, _size ) );
	}
}
