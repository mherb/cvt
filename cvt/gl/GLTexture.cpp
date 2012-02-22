/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/gl/GLTexture.h>
#include <cvt/gl/GLBuffer.h>
#include <cvt/util/SIMD.h>

namespace cvt {

		GLTexture::GLTexture( GLenum target ) : _tex( 0 ), _target( target ), _internalFormat( 0 ), _width( 0 ), _height( 0 )
		{
			glGenTextures( 1, &_tex );
		}

		GLTexture::~GLTexture()
		{
			glDeleteTextures( 1, &_tex );
		}

		void GLTexture::bind() const
		{
			glBindTexture( _target, _tex );
		}

		void GLTexture::unbind() const
		{
			glBindTexture( _target, 0 );
		}

		void GLTexture::alloc( GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* data, size_t stride )
		{
			_internalFormat = internalFormat;
			_width = width;
			_height = height;
			glBindTexture( _target, _tex );
			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
			glTexParameteri( _target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri( _target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(  _target, 0, internalFormat, width, height, 0, format, type, data );
			glBindTexture( _target, 0 );
		}


		void GLTexture::setData( GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* data, size_t stride )
		{
			glBindTexture( _target, _tex );
			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
			glTexSubImage2D(  _target, 0, xoffset, yoffset, width, height, format, type, data );
			glBindTexture( _target, 0 );
		}


		void GLTexture::toImage( Image& img, IFormatType itype ) const
		{
			GLBuffer pbo( GL_PIXEL_PACK_BUFFER );
			pbo.alloc( GL_STREAM_READ, IFormat::GRAY_FLOAT.bpp * _width * _height  );
			pbo.bind();
			bind();
			//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
			glGetTexImage( _target, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
			unbind();
			pbo.unbind();

			img.reallocate( _width, _height, IFormat::GRAY_FLOAT );

			uint8_t* src = ( uint8_t* ) pbo.map( GL_READ_ONLY );
			size_t dstride, sstride;
			uint8_t* dst = img.map( &dstride );
			uint8_t* pdst = dst;
			SIMD* simd = SIMD::instance();

			size_t n = _height;
			sstride = IFormat::GRAY_FLOAT.bpp * _width;
			while( n-- ) {
				simd->Memcpy( pdst, src, sstride );
				src += sstride;
				pdst += dstride;
			}
			pbo.unmap();
			img.unmap( dst );
		}
}
