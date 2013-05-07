/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVTGLTEXTURE_H
#define CVTGLTEXTURE_H

#include <cvt/gl/OpenGL.h>
#include <cvt/gfx/Image.h>

namespace cvt {
	class GLFBO;

	/**
	  \ingroup GL
	*/
	class GLTexture {
		friend class GLFBO;

		public:
			GLTexture( GLenum target = GL_TEXTURE_2D );
			GLTexture( const Image& img );
			~GLTexture();
			void bind() const;
			void unbind() const;
			GLenum target() const;
			void size( GLsizei& width, GLsizei& height ) const;
			GLsizei width() const;
			GLsizei height() const;
			void alloc( GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* data = NULL, size_t stride = 0 );
			void alloc( const Image& img, bool copy = true );
			void setData( GLint	xoffset, GLint yoffset,	GLsizei	width, GLsizei height, GLenum format, GLenum type, const GLvoid* data, size_t stride = 0 );
//			void setData( const GLBuffer& buffer, GLint	xoffset, GLint yoffset,	GLsizei	width, GLsizei height, GLenum format, GLenum type, const GLvoid* data, size_t stride = 0 );
			void toImage( Image& img, IFormatType itype = IFORMAT_TYPE_UINT8  ) const;

		private:
			GLTexture( const GLTexture& );
			GLTexture& operator=( const GLTexture& );

			void getGLFormat( GLenum& glformat, GLenum& gltype, const IFormat & format ) const;

			GLuint _tex;
			GLenum _target;
			GLint _internalFormat;
			GLsizei _width, _height;
	};

	inline GLenum GLTexture::target() const
	{
		return _target;
	}


	inline void GLTexture::size( GLsizei& width, GLsizei& height ) const
	{
		width = _width;
		height = _height;
	}

	inline GLsizei GLTexture::width() const
	{
		return _width;
	}

	inline GLsizei GLTexture::height() const
	{
		return _height;
	}

}

#endif

