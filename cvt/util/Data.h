#ifndef CVT_DATA_H
#define CVT_DATA_H

#include <stdint.h>

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

			void reallocate( size_t size );

		private:
			uint8_t* _data;
			size_t	 _size;
	};
}

#endif
