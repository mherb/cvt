#include <cvt/gl/GLTexture.h>

namespace cvt {

		GLTexture::GLTexture( GLenum target ) : _target( target )
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
}
