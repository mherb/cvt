/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_AUDIOBUFFER_H
#define CVT_AUDIOBUFFER_H

#include <cvt/io/audio/AudioFormat.h>
#include <cvt/util/Util.h>

namespace cvt
{
	class AudioBuffer {
		public:
			AudioBuffer( const AudioFormat& format, size_t length );
			AudioBuffer( const AudioBuffer& buffer );
			~AudioBuffer();

			const AudioFormat& format() const;

			void			   reallocate( size_t length, const AudioFormat& format );

			size_t			   length() const;
			const void*		   ptr() const;

		private:
			AudioFormat  _format;
			size_t		 _length;
			uint8_t*	 _mem;
			uint8_t*	 _buf;
	};

	inline AudioBuffer::AudioBuffer( const AudioFormat& format, size_t length ) :
		_format( format ),
		_length( length )
	{
		_mem = new uint8_t[ Math::pad16( _format.memorySize( _length ) ) ];
		_buf = Util::alignPtr( _mem, 16 );
	}

	inline AudioBuffer::~AudioBuffer()
	{
		delete[] _mem;
	}

	inline const AudioFormat& AudioBuffer::format() const
	{
		return _format;
	}

	inline size_t AudioBuffer::length() const
	{
		return _length;
	}


	inline const void* AudioBuffer::ptr() const
	{
		return _buf;
	}

}

#endif
