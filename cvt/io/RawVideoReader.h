/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_RAWVIDEO_READER
#define CVT_RAWVIDEO_READER

#include <cvt/util/String.h>
#include <cvt/io/VideoInput.h>
#include <cvt/gfx/Image.h>

namespace cvt {

	class RawVideoReader : public VideoInput
	{
		public:
			RawVideoReader( const String & fileName, bool autoRewind = true );
			~RawVideoReader();

			size_t  width() const;
			size_t  height() const;
			const   IFormat & format() const;
			const   Image & frame() const;
			bool    nextFrame( size_t timeout = 0 );
			size_t	numFrames() const { return _numFrames; }

		private:
			int			_fd;
			Image		_frame;

			size_t		_pageSize;
			size_t		_width;
			size_t		_height;
			IFormat		_format;
			bool		_autoRewind;

			void*		_map;
			size_t		_mappedSize;
			uint8_t*	_ptr;
			size_t		_stride;
			size_t		_numFrames;
			size_t		_currentFrame;

			void rewind();
			void readHeader();
	};

	inline size_t RawVideoReader::width() const
	{
		return _width;
	}

	inline size_t RawVideoReader::height() const
	{
		return _height;
	}

	inline const Image & RawVideoReader::frame() const
	{
		return _frame;
	}

	inline const IFormat & RawVideoReader::format() const
	{
		return _format;
	}

}

#endif
