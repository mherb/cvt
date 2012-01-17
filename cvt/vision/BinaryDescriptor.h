#ifndef CVT_BINARY_DESCRIPTOR_H
#define CVT_BINARY_DESCRIPTOR_H

#include <iostream>

namespace cvt
{
	template <size_t SIZE>
	class BinaryDescriptor
	{
		public:
			BinaryDescriptor();
			BinaryDescriptor( const BinaryDescriptor & other );

			size_t		   size() const	{ return  SIZE; }
			const uint8_t* data() const { return _desc; }
			      uint8_t* data()	 	{ return _desc; }

			      uint8_t& operator[]( size_t i )       { return _desc[ i ]; }
			const uint8_t& operator[]( size_t i ) const { return _desc[ i ]; }

		private:
			uint8_t	_desc[ SIZE ];
	};

	template <size_t SIZE>
	BinaryDescriptor<SIZE>::BinaryDescriptor()
	{
		_desc[ 0 ] = 0;
	}
	
	template <size_t SIZE>	
	inline BinaryDescriptor<SIZE>::BinaryDescriptor( const BinaryDescriptor & other )
	{
		for( size_t i = 0; i < SIZE; i++ ){
			_desc[ i ] = other._desc[ i ];
		}
	}

	template <size_t SIZE>
	static inline std::ostream& operator<<( std::ostream& out, const BinaryDescriptor<SIZE>& descriptor )
	{
		out << "0x";
		const uint8_t* d = descriptor.data();
		for( int i = 0; i < SIZE; i++ )
			out << std::hex << std::setfill( '0' ) << std::setw( 2 ) << (int)d[ i ];
		out << std::endl;
		return out;
	}
}

#endif
