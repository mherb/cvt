#ifndef CVT_SCOPEDBUFFER_H
#define CVT_SCOPEDBUFFER_H

#include <cvt/math/Math.h>
#include <cvt/util/Util.h>
#include <stdint.h>

namespace cvt {
	template<typename T, bool is_pod = false > class ScopedBuffer;

	template<typename T>
	class ScopedBuffer<T,false> {
		public:
			ScopedBuffer( size_t size, size_t alignment = 16 );
			~ScopedBuffer();

			size_t size() const;

			const T* ptr() const;
			T* ptr();

		private:
			ScopedBuffer( const ScopedBuffer<T>& buffer );

			size_t	   _size;
			uint8_t*   _memptr;
			T*		   _buffer;
	};

	template<typename T>
	class ScopedBuffer<T, true> {
		public:
			ScopedBuffer( size_t size, size_t alignment = 16 );
			~ScopedBuffer();

			size_t size() const;

			const T* ptr() const;
			T* ptr();

		private:
			ScopedBuffer( const ScopedBuffer<T>& buffer );

			size_t	   _size;
			uint8_t*   _memptr;
			T*		   _buffer;
	};


	template<typename T>
	inline ScopedBuffer<T, false>::ScopedBuffer( size_t size, size_t alignment ) : _size( size )
	{
        size_t memsize = Math::pad( sizeof( T ) * _size, alignment ) + alignment;// needed for 32-bit architectures!
		_memptr = new uint8_t[ memsize ];
		_buffer = Util::alignPtr<T>( ( T* ) _memptr, alignment );
		T* obj = _buffer;
		while( _size-- ) {
			obj = new( ( void* ) obj ) T();
			obj++;
		}

	}

	template<typename T>
	inline ScopedBuffer<T, false>::~ScopedBuffer()
	{
		T* obj = _buffer;
		while( _size-- ) {
			obj->~T();
			obj++;
		}
		delete[] _memptr;
	}

	template<typename T>
	inline ScopedBuffer<T, true>::ScopedBuffer( size_t size, size_t alignment ) : _size( size )
	{
        size_t memsize = Math::pad( sizeof( T ) * _size, alignment ) + alignment;// needed for 32-bit architectures!
		_memptr = new uint8_t[ memsize ];
        _buffer = Util::alignPtr<T>( ( T* ) _memptr, alignment );
	}

	template<typename T>
	inline ScopedBuffer<T, true>::~ScopedBuffer()
	{
		delete[] _memptr;
	}

	template<typename T>
	inline size_t ScopedBuffer<T,true>::size() const
	{
		return _size;
	}

	template<typename T>
	inline size_t ScopedBuffer<T,false>::size() const
	{
		return _size;
	}

	template<typename T>
	inline const T* ScopedBuffer<T,true>::ptr() const
	{
		return _buffer;
	}

	template<typename T>
	inline const T* ScopedBuffer<T,false>::ptr() const
	{
		return _buffer;
	}


	template<typename T>
	inline T* ScopedBuffer<T,true>::ptr()
	{
		return _buffer;
	}

	template<typename T>
	inline T* ScopedBuffer<T,false>::ptr()
	{
		return _buffer;
	}

}


#endif
