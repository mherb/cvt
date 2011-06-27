#ifndef CVT_VIDEO_READER
#define CVT_VIDEO_READER

#include <cvt/util/String.h>
#include <cvt/io/VideoInput.h>
#include <cvt/gfx/Image.h>

struct AVFormatContext;
struct AVCodecContext;
struct AVStream;
struct AVCodec;
struct AVFrame;
struct AVPacket;

namespace cvt {

	class VideoReader : public VideoInput
	{
		public:
			VideoReader( const String & fileName, bool autoRewind = true );
			~VideoReader();

			size_t  width() const;
			size_t  height() const;
			const   IFormat & format() const;
			const   Image & frame() const;
			void    nextFrame();

		private:
			AVFormatContext *	_formatContext;
			AVCodecContext *	_codecContext;
			AVStream *			_avStream;
			AVCodec *			_codec;
			int					_streamIndex;
			AVFrame *			_avFrame;
			Image *				_frame;
			AVPacket *			_packet;
			uint8_t *			_inBuffer;

			size_t				_width;
			size_t				_height;
			IFormat				_format;
			bool				_autoRewind;

			void updateFormat();
			void rewind();
	};

	inline size_t VideoReader::width() const
	{
		return _width;
	}

	inline size_t VideoReader::height() const
	{
		return _height;
	}

	inline const Image & VideoReader::frame() const
	{
		return *_frame;
	}

	inline const IFormat & VideoReader::format() const
	{
		return _format;
	}

}

#endif
