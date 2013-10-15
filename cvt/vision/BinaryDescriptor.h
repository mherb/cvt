/*
   The MIT License (MIT)

   Copyright (c) 2011 - 2013, Philipp Heise and Sebastian Klose

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

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
