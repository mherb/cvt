#include <cvt/io/VideoReader.h>

#include <cvt/io/FileSystem.h>
#include <cvt/util/Exception.h>

#include <iostream>

extern "C" {
	#include <avformat.h>
	#include <avcodec.h>
}

namespace cvt {

	VideoReader::VideoReader( const std::string & fileName ):
		_formatContext( 0 ),
		_codecContext( 0 ),
		_avStream( 0 ),
		_streamIndex( -1 ),
		_avFrame( 0 ),
		_frame( 0 ),
		_width( 0 ),
		_height( 0 ),
		_format( IFormat::BGRA_UINT8 )
	{
		if( !FileSystem::exists( fileName ) ){
			throw CVTException("File does not exist: " + fileName );
		}
		av_register_all();

		if( av_open_input_file( &_formatContext, fileName.c_str(), NULL, 0, NULL ) != 0 )
			throw CVTException( "Could not open file!" );

		if( av_find_stream_info( _formatContext ) < 0 )
			throw CVTException( "Could not find stream information" );

		// find first video stream:
		for( unsigned int i = 0; i < _formatContext->nb_streams; i++ ){
			if( _formatContext->streams[ i ]->codec->codec_type == CODEC_TYPE_VIDEO ){
				_streamIndex = i;
				_avStream = _formatContext->streams[ i ];
				break;
			}
		}
		if( _streamIndex == -1 )
			throw CVTException( "No video stream found in file" );

		_codecContext = _avStream->codec;
		_codec = avcodec_find_decoder( _codecContext->codec_id );

		if( _codec == NULL ){
			throw CVTException( "No appropriate codec found" );
		}

		if( avcodec_open( _codecContext, _codec ) < 0 )
			throw CVTException( "Could not open codec!" );

		_width = _codecContext->width;
		_height = _codecContext->height;
		this->updateFormat();

		_avFrame = avcodec_alloc_frame();
	}

	VideoReader::~VideoReader()
	{
		if( _frame )
			delete _frame;

		av_free( _avFrame );
		avcodec_close( _codecContext );
		av_close_input_file( _formatContext );
	}

	void VideoReader::updateFormat()
	{
		// alloc frame:
		switch( _codecContext->pix_fmt ){
			case PIX_FMT_BGRA:
				_format = IFormat::BGRA_UINT8;
				break;
			case PIX_FMT_RGBA:
				_format = IFormat::RGBA_UINT8;
				break;
			case PIX_FMT_GRAY8:
				_format = IFormat::GRAY_UINT8;
				break;
			case PIX_FMT_GRAY16LE:
				_format = IFormat::GRAY_UINT16;
				break;
			case PIX_FMT_YUV422P:
				_format = IFormat::YUYV_UINT8;
				break;
			case PIX_FMT_UYVY422:
				_format = IFormat::UYVY_UINT8;
				break;
			case PIX_FMT_YUV420P:
				_format = IFormat::YUV420_UINT8;
				break;
			default:
				std::cout << "Pixelformat:" << (int)_codecContext->pix_fmt << std::endl;
				throw CVTException( "Cannot map Pixelformat to CVT Format!" );
		}
	}

	void VideoReader::nextFrame()
	{
		int			frameFinished;

		AVPacket packet;

		while( av_read_frame( _formatContext, &packet ) >= 0 ) {
			// Is this a packet from the video stream?
			if( packet.stream_index== _streamIndex ) {
				// Decode video frame
				avcodec_decode_video( _codecContext, _avFrame, &frameFinished, packet.data, packet.size );

				// Did we get a video frame?
				if(frameFinished) {
					// decoded a new frame lying in _avFrame
					if( _codecContext->pix_fmt == PIX_FMT_YUV420P ) {
						if( !_frame )
							_frame = new Image( _width, _height, IFormat::YUV420_UINT8 );
						size_t stride;
						uint8_t* base = _frame->map<uint8_t>( &stride );
						uint8_t* dst = base;
						for( size_t i = 0; i < _height; i++ ) {
							memcpy( dst + i * stride, _avFrame->data[ 0 ] + i * _avFrame->linesize[ 0 ], _width );
						}
						dst = base + stride * _height;
						for( size_t i = 0; i < _height / 2; i++ ) {
							memcpy( dst + i * stride, _avFrame->data[ 1 ] + i * _avFrame->linesize[ 1 ], _width / 2 );
						}
						dst = base + stride * _height + _width / 2;
						for( size_t i = 0; i < _height / 2; i++ ) {
							memcpy( dst + i * stride, _avFrame->data[ 2 ] + i * _avFrame->linesize[ 2 ], _width / 2 );
						}
						_frame->unmap( base );
					}
					break;
				}
			}

			// Free the packet that was allocated by av_read_frame
			av_free_packet( &packet );
		}
		av_free_packet( &packet );
	}
}

