#ifndef CVT_RAWVIDEOWRITER_H
#define CVT_RAWVIDEOWRITER_H

#include <cvt/util/String.h>
#include <cvt/gfx/Image.h>

namespace cvt
{
	class RawVideoWriter
	{
		public:
			RawVideoWriter( const String & outname );
			~RawVideoWriter();

			void write( const Image & img );

		private:
			// file descriptor
			int		_fd;
			size_t	_currSize;
			size_t	_maxSize;


			/* current position in mapped region */
			void	* _map;
			size_t	  _mappedSize;
			uint8_t * _pos;
			size_t	  _pageSize;

			size_t	_width;
			size_t	_height;
			size_t	_stride;
			size_t	_formatID;
			size_t	_imgSize;

			/* change the filesize and update the mapping */
			void remapFile();

			/* write the header to the file */
			void writeHeader();

			/* resize to maxsize */
			void resizeFile();

	};
}

#endif
